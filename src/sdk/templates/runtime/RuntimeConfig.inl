std::string RuntimeConfigModule() {
    return R"URKCONFIG(#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "sdk/runtime_api.h"
#include "sdk/unity/unity_types.h"

namespace RuntimeConfig {
struct Value {
    enum class Kind : unsigned char { Null, Bool, Integer, Double, String, Array, Object };

    Kind kind = Kind::Null;
    bool boolean = false;
    long long integer = 0;
    double number = 0.0;
    std::string text;
    std::vector<Value> array;
    std::vector<std::pair<std::string, Value>> object;
};

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

namespace detail {

inline void log_warning(const std::string &moduleName, const std::string &message) {
    const URK::ModContext *ctx = URK::context();
    if (ctx && ctx->Log)
        ctx->Log("[config:%s] %s", moduleName.c_str(), message.c_str());
}

inline void skip_whitespace(const std::string &source, size_t &pos) {
    while (pos < source.size() && std::isspace(static_cast<unsigned char>(source[pos])))
        ++pos;
}

inline void skip_utf8_bom(const std::string &source, size_t &pos) {
    if (pos == 0 && source.size() >= 3 && static_cast<unsigned char>(source[0]) == 0xEF &&
        static_cast<unsigned char>(source[1]) == 0xBB && static_cast<unsigned char>(source[2]) == 0xBF) {
        pos = 3;
    }
}

inline bool append_unicode(std::string &output, unsigned value) {
    if (value <= 0x7f)
        output.push_back(static_cast<char>(value));
    else if (value <= 0x7ff) {
        output.push_back(static_cast<char>(0xc0 | (value >> 6)));
        output.push_back(static_cast<char>(0x80 | (value & 0x3f)));
    } else {
        output.push_back(static_cast<char>(0xe0 | (value >> 12)));
        output.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3f)));
        output.push_back(static_cast<char>(0x80 | (value & 0x3f)));
    }
    return true;
}

inline bool parse_string(const std::string &source, size_t &pos, std::string &output) {
    if (pos >= source.size() || source[pos++] != '"')
        return false;
    output.clear();
    while (pos < source.size()) {
        const char ch = source[pos++];
        if (ch == '"')
            return true;
        if (static_cast<unsigned char>(ch) < 0x20)
            return false;
        if (ch != '\\') {
            output.push_back(ch);
            continue;
        }
        if (pos >= source.size())
            return false;
        const char escape = source[pos++];
        switch (escape) {
            case '"':
                output.push_back('"');
                break;
            case '\\':
                output.push_back('\\');
                break;
            case '/':
                output.push_back('/');
                break;
            case 'b':
                output.push_back('\b');
                break;
            case 'f':
                output.push_back('\f');
                break;
            case 'n':
                output.push_back('\n');
                break;
            case 'r':
                output.push_back('\r');
                break;
            case 't':
                output.push_back('\t');
                break;
            case 'u': {
                if (pos + 4 > source.size())
                    return false;
                unsigned value = 0;
                for (int i = 0; i < 4; ++i) {
                    const char hex = source[pos++];
                    value <<= 4;
                    if (hex >= '0' && hex <= '9')
                        value |= static_cast<unsigned>(hex - '0');
                    else if (hex >= 'a' && hex <= 'f')
                        value |= static_cast<unsigned>(hex - 'a' + 10);
                    else if (hex >= 'A' && hex <= 'F')
                        value |= static_cast<unsigned>(hex - 'A' + 10);
                    else
                        return false;
                }
                if (value >= 0xd800 && value <= 0xdfff)
                    return false;
                append_unicode(output, value);
                break;
            }
            default:
                return false;
        }
    }
    return false;
}

inline bool parse_number(const std::string &source, size_t &pos, Value &output) {
    const size_t start = pos;
    bool is_double = false;
    if (pos < source.size() && source[pos] == '-')
        ++pos;
    bool digits = false;
    while (pos < source.size() && std::isdigit(static_cast<unsigned char>(source[pos]))) {
        digits = true;
        ++pos;
    }
    if (pos < source.size() && source[pos] == '.') {
        is_double = true;
        ++pos;
        while (pos < source.size() && std::isdigit(static_cast<unsigned char>(source[pos]))) {
            digits = true;
            ++pos;
        }
    }
    if (pos < source.size() && (source[pos] == 'e' || source[pos] == 'E')) {
        is_double = true;
        ++pos;
        if (pos < source.size() && (source[pos] == '+' || source[pos] == '-'))
            ++pos;
        while (pos < source.size() && std::isdigit(static_cast<unsigned char>(source[pos]))) {
            digits = true;
            ++pos;
        }
    }
    (void)start;
    if (!digits)
        return false;
    try {
        const std::string token = source.substr(start, pos - start);
        if (is_double) {
            output.kind = Value::Kind::Double;
            output.number = std::stod(token);
        } else {
            output.kind = Value::Kind::Integer;
            output.integer = std::stoll(token);
        }
        return true;
    } catch (...) {
        return false;
    }
}

inline bool parse_value(const std::string &source, size_t &pos, Value &output);

inline bool parse_object(const std::string &source, size_t &pos, Value &output) {
    output.kind = Value::Kind::Object;
    if (pos >= source.size() || source[pos++] != '{')
        return false;
    skip_whitespace(source, pos);
    while (pos < source.size() && source[pos] != '}') {
        std::string key;
        if (!parse_string(source, pos, key))
            return false;
        skip_whitespace(source, pos);
        if (pos >= source.size() || source[pos++] != ':')
            return false;
        skip_whitespace(source, pos);
        Value child;
        if (!parse_value(source, pos, child))
            return false;
        output.object.emplace_back(std::move(key), std::move(child));
        skip_whitespace(source, pos);
        if (pos < source.size() && source[pos] == ',') {
            ++pos;
            skip_whitespace(source, pos);
            continue;
        }
        if (pos >= source.size() || source[pos] != '}')
            return false;
    }
    if (pos >= source.size() || source[pos++] != '}')
        return false;
    return true;
}

inline bool parse_array(const std::string &source, size_t &pos, Value &output) {
    output.kind = Value::Kind::Array;
    if (pos >= source.size() || source[pos++] != '[')
        return false;
    skip_whitespace(source, pos);
    while (pos < source.size() && source[pos] != ']') {
        Value child;
        if (!parse_value(source, pos, child))
            return false;
        output.array.push_back(std::move(child));
        skip_whitespace(source, pos);
        if (pos < source.size() && source[pos] == ',') {
            ++pos;
            skip_whitespace(source, pos);
            continue;
        }
        if (pos >= source.size() || source[pos] != ']')
            return false;
    }
    if (pos >= source.size() || source[pos++] != ']')
        return false;
    return true;
}

inline bool parse_value(const std::string &source, size_t &pos, Value &output) {
    skip_whitespace(source, pos);
    if (pos >= source.size())
        return false;
    const char ch = source[pos];
    if (ch == '{')
        return parse_object(source, pos, output);
    if (ch == '[')
        return parse_array(source, pos, output);
    if (ch == '"')
        return parse_string(source, pos, output.text) ? (output.kind = Value::Kind::String, true) : false;
    if (source.compare(pos, 4, "true") == 0) {
        output.kind = Value::Kind::Bool;
        output.boolean = true;
        pos += 4;
        return true;
    }
    if (source.compare(pos, 5, "false") == 0) {
        output.kind = Value::Kind::Bool;
        output.boolean = false;
        pos += 5;
        return true;
    }
    if (source.compare(pos, 4, "null") == 0) {
        output.kind = Value::Kind::Null;
        pos += 4;
        return true;
    }
    if (ch == '-' || std::isdigit(static_cast<unsigned char>(ch)))
        return parse_number(source, pos, output);
    return false;
}

inline bool parse_json(const std::string &source, Value &output, std::string &error) {
    size_t pos = 0;
    skip_utf8_bom(source, pos);
    skip_whitespace(source, pos);
    if (!parse_value(source, pos, output)) {
        error = "invalid JSON";
        return false;
    }
    skip_whitespace(source, pos);
    if (pos != source.size()) {
        error = "unexpected trailing content";
        return false;
    }
    return true;
}

inline void write_escaped(std::string &output, const std::string &text) {
    output.push_back('"');
    for (const char ch : text) {
        switch (ch) {
            case '"':
                output += "\\\"";
                break;
            case '\\':
                output += "\\\\";
                break;
            case '\b':
                output += "\\b";
                break;
            case '\f':
                output += "\\f";
                break;
            case '\n':
                output += "\\n";
                break;
            case '\r':
                output += "\\r";
                break;
            case '\t':
                output += "\\t";
                break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20) {
                    char buffer[8];
                    std::snprintf(buffer, sizeof(buffer), "\\u%04x", static_cast<unsigned>(ch));
                    output += buffer;
                } else {
                    output.push_back(ch);
                }
        }
    }
    output.push_back('"');
}

inline void write_value(std::string &output, const Value &value, int depth, bool pretty) {
    const std::string indent(static_cast<std::size_t>(depth * 2), ' ');
    const std::string child_indent(static_cast<std::size_t>((depth + 1) * 2), ' ');
    switch (value.kind) {
        case Value::Kind::Null:
            output += "null";
            break;
        case Value::Kind::Bool:
            output += value.boolean ? "true" : "false";
            break;
        case Value::Kind::Integer:
            output += std::to_string(value.integer);
            break;
        case Value::Kind::Double:
            if (std::isfinite(value.number)) {
                std::ostringstream stream;
                stream.precision(9);
                stream << value.number;
                output += stream.str();
            } else {
                output += "0.0";
            }
            break;
        case Value::Kind::String:
            write_escaped(output, value.text);
            break;
        case Value::Kind::Array: {
            output.push_back('[');
            for (std::size_t i = 0; i < value.array.size(); ++i) {
                if (i > 0)
                    output.push_back(',');
                if (pretty) {
                    output.push_back('\n');
                    output += child_indent;
                }
                write_value(output, value.array[i], depth + 1, pretty);
            }
            if (!value.array.empty() && pretty) {
                output.push_back('\n');
                output += indent;
            }
            output.push_back(']');
            break;
        }
        case Value::Kind::Object: {
            output.push_back('{');
            for (std::size_t i = 0; i < value.object.size(); ++i) {
                if (i > 0)
                    output.push_back(',');
                if (pretty) {
                    output.push_back('\n');
                    output += child_indent;
                }
                write_escaped(output, value.object[i].first);
                output.push_back(':');
                if (pretty)
                    output.push_back(' ');
                write_value(output, value.object[i].second, depth + 1, pretty);
            }
            if (!value.object.empty() && pretty) {
                output.push_back('\n');
                output += indent;
            }
            output.push_back('}');
            break;
        }
    }
}

inline std::string serialize(const Value &document, bool pretty) {
    if (document.kind != Value::Kind::Object) {
        std::string root;
        write_value(root, document, 0, false);
        return root;
    }
    std::string output;
    write_value(output, document, 0, pretty);
    output.push_back('\n');
    return output;
}

inline std::filesystem::path exe_dir() {
    return URK::executable_directory();
}

inline std::filesystem::path config_directory() {
    return URK::user_data_directory() / "Configs";
}

inline std::filesystem::path config_path(const std::string &moduleName) {
    std::string safe = moduleName;
    for (char &ch : safe) {
        if (std::isspace(static_cast<unsigned char>(ch)) || ch == '/' || ch == '\\' || ch == ':' || ch == '*' ||
            ch == '?' || ch == '"' || ch == '<' || ch == '>' || ch == '|')
            ch = '_';
    }
    if (safe.empty())
        safe = "Unnamed";
    return config_directory() / (safe + ".json");
}

} // namespace detail

// The single shared in-memory document for every module, shaped like the
// .json file: an object keyed by category, then by module name, then by
// setting.
inline Value &root() {
    static Value storage;
    if (storage.kind != Value::Kind::Object)
        storage.kind = Value::Kind::Object;
    return storage;
}

inline std::filesystem::path &active_config_path() {
    static std::filesystem::path path;
    return path;
}

// Opens (or creates) the .json file for this mod (pass the mod's DLL name,
// for example "VRCMods"). The .json extension is added automatically and the
// whole file is streamed into the in-memory RuntimeConfig store, which every
// other RuntimeConfig function reads from and writes to:
//   { "Movement": { "Flight": { "speed": 1.5, ... } } }
// A missing file is created empty, and a corrupt file is left untouched.
inline bool OpenOrCreateConfig(const char *modName) {
    if (!modName || !modName[0])
        return false;
    const std::filesystem::path path = detail::config_path(modName);
    active_config_path() = path;
    const std::string safeName = path.stem().string();
    std::error_code error;
    std::filesystem::create_directories(path.parent_path(), error);
    if (error) {
        detail::log_warning(safeName, "cannot create config directory: " + error.message());
        return false;
    }
    root() = Value{};
    root().kind = Value::Kind::Object;
    std::ifstream input(path, std::ios::binary);
    if (!input)
        return true;
    std::ostringstream content;
    content << input.rdbuf();
    Value parsed;
    std::string parse_error;
    if (!detail::parse_json(content.str(), parsed, parse_error))
        return false;
    if (parsed.kind == Value::Kind::Object)
        root() = std::move(parsed);
    return true;
}

// Writes the whole in-memory store back to the .json file that was opened with
// OpenOrCreateConfig().
inline bool SaveConfig() {
    if (active_config_path().empty())
        return false;
    const std::filesystem::path path = active_config_path();
    std::error_code error;
    std::filesystem::create_directories(path.parent_path(), error);
    if (error) {
        detail::log_warning(path.stem().string(), "cannot create config directory: " + error.message());
        return false;
    }
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output)
        return false;
    output << detail::serialize(root(), true);
    return static_cast<bool>(output);
}

namespace detail {

inline const Value *find_child(const Value &document, const std::string &key) {
    if (document.kind != Value::Kind::Object)
        return nullptr;
    for (const auto &entry : document.object)
        if (entry.first == key)
            return &entry.second;
    return nullptr;
}

inline Value *find_child(Value &document, const std::string &key) {
    if (document.kind != Value::Kind::Object)
        return nullptr;
    for (auto &entry : document.object)
        if (entry.first == key)
            return &entry.second;
    return nullptr;
}

inline Value &category_document(const char *categoryName) {
    Value &document = root();
    if (Value *existing = find_child(document, categoryName))
        return *existing;
    Value created;
    created.kind = Value::Kind::Object;
    document.object.emplace_back(categoryName, std::move(created));
    return document.object.back().second;
}

inline Value &module_document(const char *categoryName, const char *moduleName) {
    Value &category = category_document(categoryName);
    if (Value *existing = find_child(category, moduleName))
        return *existing;
    Value created;
    created.kind = Value::Kind::Object;
    category.object.emplace_back(moduleName, std::move(created));
    return category.object.back().second;
}

inline const Value *find_value(const char *categoryName, const char *moduleName, const char *key) {
    if (!categoryName || !moduleName || !key || !categoryName[0] || !moduleName[0] || !key[0])
        return nullptr;
    const Value *category = find_child(root(), categoryName);
    if (!category)
        return nullptr;
    const Value *module = find_child(*category, moduleName);
    if (!module)
        return nullptr;
    return find_child(*module, key);
}

} // namespace detail

inline bool Has(const char *categoryName, const char *moduleName, const char *key) {
    return detail::find_value(categoryName, moduleName, key) != nullptr;
}

inline bool Remove(const char *categoryName, const char *moduleName, const char *key) {
    if (!categoryName || !moduleName || !key || !categoryName[0] || !moduleName[0] || !key[0])
        return false;
    Value *category = detail::find_child(root(), categoryName);
    if (!category)
        return false;
    Value *module = detail::find_child(*category, moduleName);
    if (!module || module->kind != Value::Kind::Object)
        return false;
    const std::string name(key);
    const auto end = std::remove_if(module->object.begin(), module->object.end(),
                                    [&](const std::pair<std::string, Value> &entry) { return entry.first == name; });
    const bool removed = end != module->object.end();
    module->object.erase(end, module->object.end());
    return removed;
}

namespace detail {

inline bool coerce_bool(const Value &value, bool fallback) {
    switch (value.kind) {
        case Value::Kind::Bool:
            return value.boolean;
        case Value::Kind::Integer:
            return value.integer != 0;
        case Value::Kind::Double:
            return value.number != 0.0;
        case Value::Kind::String:
            return value.text == "true" || value.text == "1";
        default:
            return fallback;
    }
}

inline long long coerce_integer(const Value &value, long long fallback) {
    switch (value.kind) {
        case Value::Kind::Integer:
            return value.integer;
        case Value::Kind::Double:
            return static_cast<long long>(value.number);
        case Value::Kind::Bool:
            return value.boolean ? 1 : 0;
        case Value::Kind::String: {
            try {
                return std::stoll(value.text);
            } catch (...) {
                return fallback;
            }
        }
        default:
            return fallback;
    }
}

inline double coerce_double(const Value &value, double fallback) {
    switch (value.kind) {
        case Value::Kind::Double:
            return value.number;
        case Value::Kind::Integer:
            return static_cast<double>(value.integer);
        case Value::Kind::Bool:
            return value.boolean ? 1.0 : 0.0;
        case Value::Kind::String: {
            try {
                return std::stod(value.text);
            } catch (...) {
                return fallback;
            }
        }
        default:
            return fallback;
    }
}

inline std::string coerce_string(const Value &value, const char *fallback) {
    switch (value.kind) {
        case Value::Kind::String:
            return value.text;
        case Value::Kind::Integer:
            return std::to_string(value.integer);
        case Value::Kind::Double:
            if (std::isfinite(value.number)) {
                std::ostringstream stream;
                stream.precision(9);
                stream << value.number;
                return stream.str();
            }
            return fallback ? fallback : "";
        case Value::Kind::Bool:
            return value.boolean ? "true" : "false";
        default:
            return fallback ? fallback : "";
    }
}

} // namespace detail

inline bool GetBool(const char *categoryName, const char *moduleName, const char *key, bool fallback = false) {
    const Value *value = detail::find_value(categoryName, moduleName, key);
    return value ? detail::coerce_bool(*value, fallback) : fallback;
}

inline int GetInt(const char *categoryName, const char *moduleName, const char *key, int fallback = 0) {
    const Value *value = detail::find_value(categoryName, moduleName, key);
    return value ? static_cast<int>(detail::coerce_integer(*value, fallback)) : fallback;
}

inline float GetFloat(const char *categoryName, const char *moduleName, const char *key, float fallback = 0.0f) {
    const Value *value = detail::find_value(categoryName, moduleName, key);
    return value ? static_cast<float>(detail::coerce_double(*value, fallback)) : fallback;
}

inline double GetDouble(const char *categoryName, const char *moduleName, const char *key, double fallback = 0.0) {
    const Value *value = detail::find_value(categoryName, moduleName, key);
    return value ? detail::coerce_double(*value, fallback) : fallback;
}

inline std::string GetString(const char *categoryName, const char *moduleName, const char *key, const char *fallback = "") {
    const Value *value = detail::find_value(categoryName, moduleName, key);
    return value ? detail::coerce_string(*value, fallback) : (fallback ? fallback : "");
}

template <typename ColorT>
inline bool GetColor(const char *categoryName, const char *moduleName, const char *key, ColorT &output,
                     const ColorT &fallback = {}) {
    const Value *value = detail::find_value(categoryName, moduleName, key);
    if (!value || value->kind != Value::Kind::Array) {
        output = fallback;
        return false;
    }
    ColorT result{};
    const bool valid = value->array.size() >= 4;
    if (value->array.size() > 0)
        result.r = static_cast<float>(detail::coerce_double(value->array[0], fallback.r));
    if (value->array.size() > 1)
        result.g = static_cast<float>(detail::coerce_double(value->array[1], fallback.g));
    if (value->array.size() > 2)
        result.b = static_cast<float>(detail::coerce_double(value->array[2], fallback.b));
    if (value->array.size() > 3)
        result.a = static_cast<float>(detail::coerce_double(value->array[3], fallback.a));
    output = result;
    return valid;
}

namespace detail {

inline void set_entry(Value &document, const char *key, Value entry) {
    if (document.kind != Value::Kind::Object)
        document.kind = Value::Kind::Object;
    for (auto &pair : document.object)
        if (pair.first == key) {
            pair.second = std::move(entry);
            return;
        }
    document.object.emplace_back(key, std::move(entry));
}

} // namespace detail

inline void SetBool(const char *categoryName, const char *moduleName, const char *key, bool value) {
    Value entry;
    entry.kind = Value::Kind::Bool;
    entry.boolean = value;
    detail::set_entry(detail::module_document(categoryName, moduleName), key, std::move(entry));
}

inline void SetInt(const char *categoryName, const char *moduleName, const char *key, long long value) {
    Value entry;
    entry.kind = Value::Kind::Integer;
    entry.integer = value;
    detail::set_entry(detail::module_document(categoryName, moduleName), key, std::move(entry));
}

inline void SetFloat(const char *categoryName, const char *moduleName, const char *key, float value) {
    Value entry;
    entry.kind = Value::Kind::Double;
    entry.number = static_cast<double>(value);
    detail::set_entry(detail::module_document(categoryName, moduleName), key, std::move(entry));
}

inline void SetDouble(const char *categoryName, const char *moduleName, const char *key, double value) {
    Value entry;
    entry.kind = Value::Kind::Double;
    entry.number = value;
    detail::set_entry(detail::module_document(categoryName, moduleName), key, std::move(entry));
}

inline void SetString(const char *categoryName, const char *moduleName, const char *key, const std::string &value) {
    Value entry;
    entry.kind = Value::Kind::String;
    entry.text = value;
    detail::set_entry(detail::module_document(categoryName, moduleName), key, std::move(entry));
}

template <typename ColorT>
inline void SetColor(const char *categoryName, const char *moduleName, const char *key, const ColorT &value) {
    Value entry;
    entry.kind = Value::Kind::Array;
    Value channels[4];
    for (int i = 0; i < 4; ++i) {
        channels[i].kind = Value::Kind::Double;
        channels[i].number = (i == 0) ? value.r : (i == 1) ? value.g : (i == 2) ? value.b : value.a;
        entry.array.push_back(channels[i]);
    }
    detail::set_entry(detail::module_document(categoryName, moduleName), key, std::move(entry));
}
} // namespace RuntimeConfig
)URKCONFIG";
}