#include "il2cpp_symbol_map.h"

#include "logger.h"

#include <cctype>
#include <fstream>
#include <vector>

namespace {
// ---------------------------------------------------------------------------
// Minimal JSON parser
//
// The loader DLLs do not link a JSON library (nlohmann is dev-tool only), so
// this is a compact, non-allocating-friendly recursive descent parser that
// accepts the object/array/string schema symbols.json uses. Numbers, bools and
// nulls are consumed and ignored; strings may contain the standard \" \\ \/
// \b \f \n \r \t and \uXXXX escapes.
// ---------------------------------------------------------------------------

struct JsonValue {
    enum class Kind { Null, String, Object, Array };
    Kind kind = Kind::Null;
    std::string string;
    // Object: alternating keys and values; Array: plain values.
    std::vector<std::string> keys;
    std::vector<JsonValue> children;
};

class JsonParser {
  public:
    explicit JsonParser(const char *text) : m_text(text), m_pos(text) {}

    bool Parse(JsonValue &root) {
        SkipWhitespace();
        if (!ParseValue(root)) {
            return false;
        }
        SkipWhitespace();
        return *m_pos == '\0';
    }

  private:
    const std::string m_text;
    const char *m_pos;

    void SkipWhitespace() {
        while (*m_pos == ' ' || *m_pos == '\t' || *m_pos == '\r' || *m_pos == '\n')
            ++m_pos;
    }

    bool ParseValue(JsonValue &value) {
        SkipWhitespace();
        switch (*m_pos) {
        case '"':
            value.kind = JsonValue::Kind::String;
            return ParseString(value.string);
        case '{':
            return ParseObject(value);
        case '[':
            return ParseArray(value);
        case 't':
            if (ConsumeLiteral("true")) {
                value.kind = JsonValue::Kind::Null;
                return true;
            }
            return false;
        case 'f':
            if (ConsumeLiteral("false")) {
                value.kind = JsonValue::Kind::Null;
                return true;
            }
            return false;
        case 'n':
            if (ConsumeLiteral("null")) {
                value.kind = JsonValue::Kind::Null;
                return true;
            }
            return false;
        default:
            if (*m_pos == '-' || (*m_pos >= '0' && *m_pos <= '9')) {
                value.kind = JsonValue::Kind::Null;
                return ConsumeNumber();
            }
            return false;
        }
    }

    bool ConsumeLiteral(const char *literal) {
        const size_t length = std::strlen(literal);
        if (std::strncmp(m_pos, literal, length) != 0)
            return false;
        m_pos += length;
        return true;
    }

    bool ConsumeNumber() {
        if (*m_pos == '-')
            ++m_pos;
        if (*m_pos == '0') {
            ++m_pos;
        } else if (*m_pos >= '1' && *m_pos <= '9') {
            while (*m_pos >= '0' && *m_pos <= '9')
                ++m_pos;
        } else {
            return false;
        }
        if (*m_pos == '.') {
            ++m_pos;
            if (*m_pos < '0' || *m_pos > '9')
                return false;
            while (*m_pos >= '0' && *m_pos <= '9')
                ++m_pos;
        }
        if (*m_pos == 'e' || *m_pos == 'E') {
            ++m_pos;
            if (*m_pos == '+' || *m_pos == '-')
                ++m_pos;
            if (*m_pos < '0' || *m_pos > '9')
                return false;
            while (*m_pos >= '0' && *m_pos <= '9')
                ++m_pos;
        }
        return true;
    }

    bool ParseString(std::string &out) {
        if (*m_pos != '"')
            return false;
        ++m_pos;
        out.clear();
        while (*m_pos != '\0') {
            const char ch = *m_pos++;
            if (ch == '"')
                return true;
            if (ch != '\\') {
                out += ch;
                continue;
            }
            if (*m_pos == '\0')
                return false;
            const char esc = *m_pos++;
            switch (esc) {
            case '"': out += '"'; break;
            case '\\': out += '\\'; break;
            case '/': out += '/'; break;
            case 'b': out += '\b'; break;
            case 'f': out += '\f'; break;
            case 'n': out += '\n'; break;
            case 'r': out += '\r'; break;
            case 't': out += '\t'; break;
            case 'u':
                if (!ParseUnicodeEscape(out))
                    return false;
                break;
            default:
                return false;
            }
        }
        return false;
    }

    bool ParseUnicodeEscape(std::string &out) {
        unsigned int codepoint = 0;
        for (int i = 0; i < 4; ++i) {
            const char ch = *m_pos++;
            codepoint <<= 4;
            if (ch >= '0' && ch <= '9')
                codepoint |= static_cast<unsigned int>(ch - '0');
            else if (ch >= 'a' && ch <= 'f')
                codepoint |= static_cast<unsigned int>(ch - 'a' + 10);
            else if (ch >= 'A' && ch <= 'F')
                codepoint |= static_cast<unsigned int>(ch - 'A' + 10);
            else
                return false;
        }
        if (codepoint <= 0x7F) {
            out += static_cast<char>(codepoint);
        } else if (codepoint <= 0x7FF) {
            out += static_cast<char>(0xC0 | (codepoint >> 6));
            out += static_cast<char>(0x80 | (codepoint & 0x3F));
        } else {
            out += static_cast<char>(0xE0 | (codepoint >> 12));
            out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        return true;
    }

    bool ParseObject(JsonValue &value) {
        value.kind = JsonValue::Kind::Object;
        ++m_pos; // '{'
        SkipWhitespace();
        if (*m_pos == '}') {
            ++m_pos;
            return true;
        }
        while (true) {
            SkipWhitespace();
            std::string key;
            if (!ParseString(key))
                return false;
            SkipWhitespace();
            if (*m_pos != ':')
                return false;
            ++m_pos;
            JsonValue child;
            if (!ParseValue(child))
                return false;
            value.keys.push_back(std::move(key));
            value.children.push_back(std::move(child));
            SkipWhitespace();
            if (*m_pos == ',') {
                ++m_pos;
                continue;
            }
            if (*m_pos == '}') {
                ++m_pos;
                return true;
            }
            return false;
        }
    }

    bool ParseArray(JsonValue &value) {
        value.kind = JsonValue::Kind::Array;
        ++m_pos; // '['
        SkipWhitespace();
        if (*m_pos == ']') {
            ++m_pos;
            return true;
        }
        while (true) {
            SkipWhitespace();
            JsonValue child;
            if (!ParseValue(child))
                return false;
            value.children.push_back(std::move(child));
            SkipWhitespace();
            if (*m_pos == ',') {
                ++m_pos;
                continue;
            }
            if (*m_pos == ']') {
                ++m_pos;
                return true;
            }
            return false;
        }
    }
};

// Reads a whole file into a heap string; null-safe, empty on failure.
std::string ReadFile(const char *path) {
    if (!path || !*path)
        return {};
    std::ifstream stream(path, std::ios::binary);
    if (!stream.is_open())
        return {};
    std::string contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    if (contents.size() >= 3 && static_cast<unsigned char>(contents[0]) == 0xEF &&
        static_cast<unsigned char>(contents[1]) == 0xBB && static_cast<unsigned char>(contents[2]) == 0xBF) {
        contents.erase(0, 3); // UTF-8 BOM
    }
    return contents;
}

const JsonValue *FindMember(const JsonValue *object, const char *name) {
    if (!object || object->kind != JsonValue::Kind::Object)
        return nullptr;
    for (size_t i = 0; i < object->keys.size(); ++i) {
        if (object->keys[i] == name)
            return &object->children[i];
    }
    return nullptr;
}

const JsonValue *StringValueOf(const JsonValue *value, std::string &out) {
    if (!value || value->kind != JsonValue::Kind::String)
        return nullptr;
    out = value->string;
    return value;
}

// Parses an optional string->string member object into out; only string values
// are kept, everything else is counted as a malformed record.
void ParseMemberMap(const JsonValue *value, std::unordered_map<std::string, std::string> &out,
                    size_t *invalid) {
    if (!value)
        return;
    if (value->kind != JsonValue::Kind::Object) {
        if (invalid)
            ++(*invalid);
        return;
    }
    for (size_t i = 0; i < value->keys.size(); ++i) {
        const JsonValue *member = &value->children[i];
        if (member->kind == JsonValue::Kind::String && !member->string.empty())
            out[value->keys[i]] = member->string;
        else if (invalid)
            ++(*invalid);
    }
}

const char *ResolveFromMap(const std::unordered_map<std::string, std::string> &map, const char *obfName) {
    if (!obfName)
        return nullptr;
    const auto it = map.find(obfName);
    return it != map.end() && !it->second.empty() ? it->second.c_str() : nullptr;
}
} // namespace

bool Il2Cpp_LoadSymbolMap(const char *path, Il2CppSymbolMap &map) {
    map.classes.clear();
    map.invalidEntries = 0;

    const std::string contents = ReadFile(path);
    if (contents.empty())
        return false;

    JsonValue root;
    JsonParser parser(contents.c_str());
    if (!parser.Parse(root)) {
        Log("[IL2CPP][ERROR] Symbol map %s is not valid JSON.", path ? path : "<null>");
        return false;
    }

    const JsonValue *assemblies = FindMember(&root, "assemblies");
    if (!assemblies || assemblies->kind != JsonValue::Kind::Array) {
        Log("[IL2CPP][ERROR] Symbol map %s has no 'assemblies' array.",
            path ? path : "<null>");
        return false;
    }

    for (const JsonValue &assembly : assemblies->children) {
        const JsonValue *classes = FindMember(&assembly, "classes");
        if (!classes || classes->kind != JsonValue::Kind::Array)
            continue;
        for (const JsonValue &classValue : classes->children) {
            std::string obfuscated;
            if (!StringValueOf(FindMember(&classValue, "obfuscated"), obfuscated) || obfuscated.empty()) {
                ++map.invalidEntries;
                continue;
            }
            Il2CppClassSymbols symbols;
            std::string realName;
            if (StringValueOf(FindMember(&classValue, "name"), realName) && !realName.empty())
                symbols.realName = realName;
            // 'namespace' and 'assembly' are carried for context/documentation;
            // resolution is keyed on the obfuscated class name alone.
            ParseMemberMap(FindMember(&classValue, "methods"), symbols.methods, &map.invalidEntries);
            ParseMemberMap(FindMember(&classValue, "fields"), symbols.fields, &map.invalidEntries);
            ParseMemberMap(FindMember(&classValue, "properties"), symbols.properties, &map.invalidEntries);
            ParseMemberMap(FindMember(&classValue, "events"), symbols.events, &map.invalidEntries);
            map.classes[obfuscated] = std::move(symbols);
        }
    }

    if (map.classes.empty()) {
        Log("[IL2CPP][ERROR] Symbol map %s contained no usable class records.",
            path ? path : "<null>");
        return false;
    }
    return true;
}

const char *Il2Cpp_ResolveClassName(const Il2CppSymbolMap &map, const char *obfClassName) {
    const auto it = map.classes.find(obfClassName ? obfClassName : "");
    if (it == map.classes.end() || it->second.realName.empty())
        return nullptr;
    return it->second.realName.c_str();
}

const char *Il2Cpp_ResolveMemberName(const Il2CppSymbolMap &map, Il2CppSymbolKind kind,
                                     const char *obfClassName, const char *obfMemberName) {
    const auto it = map.classes.find(obfClassName ? obfClassName : "");
    if (it == map.classes.end())
        return nullptr;
    switch (kind) {
    case Il2CppSymbolKind::Method:
        return ResolveFromMap(it->second.methods, obfMemberName);
    case Il2CppSymbolKind::Field:
        return ResolveFromMap(it->second.fields, obfMemberName);
    case Il2CppSymbolKind::Property:
        return ResolveFromMap(it->second.properties, obfMemberName);
    case Il2CppSymbolKind::Event:
        return ResolveFromMap(it->second.events, obfMemberName);
    }
    return nullptr;
}