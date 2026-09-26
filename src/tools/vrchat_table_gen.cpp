// Turns a live metadata capture into VRChat generator table entries.
//
// The table in vrchat_generated_table.inl is the hand-maintained seam between
// "what VRChat's metadata says" and "what the generator emits". Writing it by
// hand is how signatures drift, so this reads the same JSON the runtime explorer
// produces and emits the entries mechanically.
//
// The type mapping is taken from vrchat_generated_emit.inl rather than repeated
// here, so a type the emitter cannot map is reported rather than silently
// emitted as void*.
//
// Usage: urk-vrchat-table-gen <capture.json> [--emit-all]
//   default   emit entries for the types in the capture
//   --emit-all also emit every known type, to regenerate the whole table

#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "src/sdk/templates/VRChat/vrchat_generated_emit.inl"

namespace {

// A deliberately small JSON reader. The capture is machine generated with a
// stable shape, so a full parser would be a dependency for no benefit.
struct Json {
    enum class Kind { Null, Bool, Number, String, Array, Object };
    Kind kind = Kind::Null;
    bool boolean = false;
    double number = 0;
    std::string text;
    std::vector<Json> items;
    std::vector<std::pair<std::string, Json>> members;

    const Json *find(const std::string &key) const {
        for (const auto &entry : members)
            if (entry.first == key) return &entry.second;
        return nullptr;
    }
    std::string str(const std::string &key, const std::string &fallback = {}) const {
        const Json *v = find(key);
        return v && v->kind == Kind::String ? v->text : fallback;
    }
    bool flag(const std::string &key, bool fallback = false) const {
        const Json *v = find(key);
        return v && v->kind == Kind::Bool ? v->boolean : fallback;
    }
    std::vector<Json> list(const std::string &key) const {
        const Json *v = find(key);
        if (!v || v->kind != Kind::Array) return {};
        return v->items;
    }
};

class Parser {
  public:
    explicit Parser(const std::string &source) : src_(source) {}

    bool parse(Json &out) {
        skip();
        if (!value(out)) return false;
        return true;
    }
    const std::string &error() const { return error_; }

  private:
    const std::string &src_;
    size_t at_ = 0;
    std::string error_;

    void skip() {
        while (at_ < src_.size() &&
               (src_[at_] == ' ' || src_[at_] == '\n' || src_[at_] == '\r' || src_[at_] == '\t'))
            ++at_;
    }
    bool fail(const std::string &why) {
        if (error_.empty()) error_ = why + " at offset " + std::to_string(at_);
        return false;
    }
    bool literal(const char *word) {
        const size_t n = std::strlen(word);
        if (src_.compare(at_, n, word) != 0) return false;
        at_ += n;
        return true;
    }
    bool string(std::string &out) {
        if (at_ >= src_.size() || src_[at_] != '"') return fail("expected string");
        ++at_;
        out.clear();
        while (at_ < src_.size() && src_[at_] != '"') {
            char c = src_[at_++];
            if (c == '\\' && at_ < src_.size()) {
                const char esc = src_[at_++];
                switch (esc) {
                    case 'n': out += '\n'; break;
                    case 't': out += '\t'; break;
                    case 'r': out += '\r'; break;
                    case 'b': out += '\b'; break;
                    case 'f': out += '\f'; break;
                    case 'u': {
                        // Only the BMP subset the capture actually uses.
                        if (at_ + 4 > src_.size()) return fail("truncated \\u escape");
                        const std::string hex = src_.substr(at_, 4);
                        at_ += 4;
                        const unsigned code =
                            static_cast<unsigned>(std::stoul(hex, nullptr, 16));
                        if (code < 0x80) {
                            out += static_cast<char>(code);
                        } else if (code < 0x800) {
                            out += static_cast<char>(0xC0 | (code >> 6));
                            out += static_cast<char>(0x80 | (code & 0x3F));
                        } else {
                            out += static_cast<char>(0xE0 | (code >> 12));
                            out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
                            out += static_cast<char>(0x80 | (code & 0x3F));
                        }
                        break;
                    }
                    default: out += esc; break;
                }
            } else {
                out += c;
            }
        }
        if (at_ >= src_.size()) return fail("unterminated string");
        ++at_;
        return true;
    }
    bool value(Json &out) {
        skip();
        if (at_ >= src_.size()) return fail("unexpected end");
        const char c = src_[at_];

        if (c == '{') {
            ++at_;
            out.kind = Json::Kind::Object;
            skip();
            if (at_ < src_.size() && src_[at_] == '}') { ++at_; return true; }
            for (;;) {
                skip();
                std::string key;
                if (!string(key)) return false;
                skip();
                if (at_ >= src_.size() || src_[at_] != ':') return fail("expected ':'");
                ++at_;
                Json child;
                if (!value(child)) return false;
                out.members.emplace_back(std::move(key), std::move(child));
                skip();
                if (at_ < src_.size() && src_[at_] == ',') { ++at_; continue; }
                if (at_ < src_.size() && src_[at_] == '}') { ++at_; return true; }
                return fail("expected ',' or '}'");
            }
        }

        if (c == '[') {
            ++at_;
            out.kind = Json::Kind::Array;
            skip();
            if (at_ < src_.size() && src_[at_] == ']') { ++at_; return true; }
            for (;;) {
                Json child;
                if (!value(child)) return false;
                out.items.push_back(std::move(child));
                skip();
                if (at_ < src_.size() && src_[at_] == ',') { ++at_; continue; }
                if (at_ < src_.size() && src_[at_] == ']') { ++at_; return true; }
                return fail("expected ',' or ']'");
            }
        }

        if (c == '"') {
            out.kind = Json::Kind::String;
            return string(out.text);
        }
        if (literal("true")) { out.kind = Json::Kind::Bool; out.boolean = true; return true; }
        if (literal("false")) { out.kind = Json::Kind::Bool; out.boolean = false; return true; }
        if (literal("null")) { out.kind = Json::Kind::Null; return true; }

        // Number
        const size_t start = at_;
        while (at_ < src_.size() && (std::isdigit(static_cast<unsigned char>(src_[at_])) ||
                                     src_[at_] == '-' || src_[at_] == '+' ||
                                     src_[at_] == '.' || src_[at_] == 'e' || src_[at_] == 'E'))
            ++at_;
        if (start == at_) return fail("unexpected character");
        out.kind = Json::Kind::Number;
        out.number = std::stod(src_.substr(start, at_ - start));
        return true;
    }
};

std::string Escape(const std::string &in) {
    std::string out;
    for (const char c : in) {
        if (c == '"' || c == '\\') { out += '\\'; out += c; }
        else if (c == '\n') out += "\\n";
        else out += c;
    }
    return out;
}

// The emitter maps an unrecognised managed type to void*. That still compiles,
// and it is what the hand-written table already does for collections and
// arrays, but it loses the element type. So members are classified rather than
// dropped: exact ones are trustworthy, opaque ones are emitted and reported.
enum class Fidelity { Exact, Opaque, Unsupported };

Fidelity Classify(const std::string &managed) {
    if (managed.empty()) return Fidelity::Unsupported;
    if (managed == "System.Void") return Fidelity::Exact;
    if (VrcGenerated::IsManagedEnum(managed)) return Fidelity::Exact;
    const std::string cpp = VrcGenerated::ManagedToCpp(managed);
    if (cpp == "void*") return Fidelity::Opaque;
    // ManagedToCpp resolves what it knows through its own tables, so anything
    // coming back unchanged is a type it does not know at all.
    return cpp == managed ? Fidelity::Unsupported : Fidelity::Exact;
}

struct Member {
    std::string name;
    std::string type;
    bool is_static = false;
};

struct Method {
    std::string name;
    std::string return_type;
    std::vector<std::string> params;
    bool is_static = false;
};

struct TypeEntry {
    std::string managed;     // VRC.SDKBase.VRC_Trigger
    std::string cpp_name;    // VrcTrigger
    std::string cpp_ns;      // VRC::SDKBase
    std::string image;
    std::string ns;
    std::string cls;
    bool is_enum = false;
    bool is_interface = false;
    bool is_static_class = false;
    bool is_value_type = false;
    bool is_component = false;
    std::vector<std::string> enum_names;
    std::vector<Member> fields;
    std::vector<Member> properties;
    std::vector<Method> methods;
    std::set<std::string> method_sigs;
    std::vector<std::string> skipped;
};

// VRC.SDKBase.VRC_Trigger -> "Trigger". The leading VRC_ belongs to the managed
// name, and keeping it produced a VrcVRC_Trigger symbol.
std::string PascalCase(const std::string &managed) {
    const size_t dot = managed.rfind('.');
    std::string name = dot == std::string::npos ? managed : managed.substr(dot + 1);
    if (name.rfind("VRC_", 0) == 0) name = name.substr(4);

    std::string out;
    bool upper = true;
    for (const char c : name) {
        if (c == '_') { out += c; upper = true; continue; }
        out += upper ? static_cast<char>(std::toupper(static_cast<unsigned char>(c)))
                     : c;
        upper = false;
    }
    return out;
}

// VRC.SDKBase -> VRC::SDKBase, VRC.Core.Annotations -> VRC::Core::Annotations.
// The capture's ns already carries the VRC root, so prefixing "VRC::" blindly
// produced the malformed VRC::VRC.SDKBase.
std::string CppNamespace(const std::string &ns) {
    if (ns == "VRC" || ns.empty()) return "VRC";
    std::string rest = ns.rfind("VRC.", 0) == 0 ? ns.substr(4) : ns;
    std::string out = "VRC::";
    for (const char c : rest) {
        if (c == '.') out += "::";
        else out += c;
    }
    return out;
}

// The symbol for a method's parameter array. IL2CPP methods can be overloaded,
// and the params array is a named global, so the index is part of the symbol
// while the emitted method name stays the real one.
std::string ParamsSymbol(const TypeEntry &entry, size_t index) {
    std::string symbol = entry.cpp_name + entry.methods[index].name;
    size_t seen = 0;
    for (size_t j = 0; j < index; ++j)
        if (entry.methods[j].name == entry.methods[index].name) ++seen;
    while (seen-- > 0) symbol += "_";
    return symbol;
}

// VRC.SDKBase.VRC_Trigger -> sdk/VRChat/VRC/SDKBase/VRC_Trigger.h
// The sdk/VRChat/VRC root already stands in for the VRC namespace, so the VRC
// prefix is dropped from the remainder to avoid sdk/VRChat/VRC/VRC/...
std::string HeaderPath(const TypeEntry &entry) {
    const std::string ns = entry.ns.rfind("VRC.", 0) == 0 ? entry.ns.substr(4) : entry.ns;
    std::string out = "sdk/VRChat/VRC/";
    for (const char c : ns) out += c == '.' ? '/' : c;
    out += "/";
    out += entry.cls;
    out += ".h";
    return out;
}

} // namespace

int main(int argc, char **argv) {
    if (argc < 2) {
        std::fprintf(stderr,
                     "usage: urk-vrchat-table-gen <capture.json> [--factories] [--writes]\n");
        return 2;
    }

    bool want_factories = false;
    bool want_writes = false;
    std::set<std::string> excluded;
    std::set<std::string> reserved;
    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--factories") want_factories = true;
        else if (arg == "--writes") want_writes = true;
        else if (arg == "--exclude" && i + 1 < argc) {
            std::string list = argv[++i];
            for (size_t at = list.find(',');; at = list.find(',', at + 1)) {
                excluded.insert(list.substr(0, at));
                if (at == std::string::npos) break;
                list.erase(0, at + 1);
            }
        } else if (arg == "--reserve-file" && i + 1 < argc) {
            // Header paths already claimed by a hand-written template. The
            // generator rejects a duplicate path, and a type that resolves to one
            // of these has to stay hand written.
            std::ifstream list(argv[++i]);
            if (!list) {
                std::fprintf(stderr, "cannot open reserve list %s\n", argv[i]);
                return 2;
            }
            std::string line;
            while (std::getline(list, line)) {
                while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
                size_t start = line.find_first_not_of(" \t");
                if (start == std::string::npos || line[start] == '#') continue;
                // The ledger treats paths case insensitively, so a reserve that
                // is not would miss VRCPlayerApi against VRCPlayerAPI.h.
                for (char &c : line) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                reserved.insert(line);
            }
        } else if (arg == "--exclude-file" && i + 1 < argc) {
            // A file rather than an argument list: a long comma-joined list is
            // at the mercy of however the shell chooses to split it.
            std::ifstream list(argv[++i]);
            if (!list) {
                std::fprintf(stderr, "cannot open exclude list %s\n", argv[i]);
                return 2;
            }
            std::string line;
            while (std::getline(list, line)) {
                while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
                size_t start = line.find_first_not_of(" \t");
                if (start == std::string::npos || line[start] == '#') continue;
                excluded.insert(line.substr(start));
            }
        } else {
            std::fprintf(stderr, "unknown option %s\n", arg.c_str());
            return 2;
        }
    }

    std::ifstream in(argv[1], std::ios::binary);
    if (!in) {
        std::fprintf(stderr, "cannot open %s\n", argv[1]);
        return 2;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    const std::string source = buffer.str();

    Json root;
    Parser parser(source);
    if (!parser.parse(root) || root.kind != Json::Kind::Object) {
        std::fprintf(stderr, "parse failed: %s\n", parser.error().c_str());
        return 1;
    }

    const Json *types = root.find("types");
    if (!types || types->kind != Json::Kind::Object) {
        std::fprintf(stderr, "capture has no 'types' object\n");
        return 1;
    }

    std::vector<TypeEntry> entries;
    std::set<std::string> unsupported;
    std::set<std::string> opaque;

    for (const auto &[managed, value] : types->members) {
        if (value.kind != Json::Kind::Object) continue;
        // The hand-written table entries carry alt_name resilience, so they win
        // over anything regenerated here.
        if (excluded.count(managed) != 0) {
            std::fprintf(stderr, "  excluded (hand-written) %s\n", managed.c_str());
            continue;
        }
        // A managed type can land on a path a hand-written template already owns.
        // Comparing the rendered path, not the managed name, is what catches the
        // case-insensitive and renamed variants.
        {
            TypeEntry probe;
            probe.ns = value.str("ns");
            probe.cls = value.str("cls", managed.substr(managed.rfind('.') + 1));
            const std::string path = HeaderPath(probe);
            std::string folded = path;
            for (char &c : folded) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (reserved.count(folded) != 0) {
                std::fprintf(stderr, "  excluded (path taken) %s -> %s\n", managed.c_str(),
                             path.c_str());
                continue;
            }
        }

        TypeEntry entry;
        entry.managed = managed;
        entry.image = value.str("image");
        entry.ns = value.str("ns");
        entry.cls = value.str("cls");
        entry.is_enum = VrcGenerated::IsManagedEnum(managed);
        entry.is_interface = value.flag("iface");
        entry.is_static_class = value.flag("static");
        entry.is_value_type = !entry.is_static_class && !value.flag("component") &&
                              !entry.is_interface && !entry.is_enum;
        entry.is_component = value.flag("component") && !entry.is_interface && !entry.is_enum;
        entry.cpp_name = "Vrc" + PascalCase(managed);
        entry.cpp_ns = CppNamespace(entry.ns);

        for (const auto &field : value.list("field")) {
            Member m;
            m.name = field.str("n");
            m.type = field.str("t");
            m.is_static = field.flag("s");
            if (m.name.empty()) continue;
            if (VrcGenerated::IsSkippableField(m.name, m.type)) continue;
            if (!entry.is_enum) {
                const Fidelity f = Classify(m.type);
                if (f == Fidelity::Unsupported) {
                    unsupported.insert(m.type);
                    entry.skipped.push_back("field " + m.name + " : " + m.type);
                    continue;
                }
                if (f == Fidelity::Opaque) opaque.insert(m.type);
            }
            entry.fields.push_back(std::move(m));
        }

        for (const auto &prop : value.list("property")) {
            Member m;
            m.name = prop.str("n");
            m.type = prop.str("t");
            m.is_static = prop.flag("s");
            if (m.name.empty()) continue;
            if (VrcGenerated::IsSkippableField(m.name, m.type)) continue;
            const Fidelity f = Classify(m.type);
            if (f == Fidelity::Unsupported) {
                unsupported.insert(m.type);
                entry.skipped.push_back("property " + m.name + " : " + m.type);
                continue;
            }
            if (f == Fidelity::Opaque) opaque.insert(m.type);
            entry.properties.push_back(std::move(m));
        }

        for (const auto &method : value.list("methods")) {
            Method m;
            m.name = method.str("n");
            m.return_type = method.str("rt");
            m.is_static = method.flag("s");
            for (const auto &p : method.list("pt")) {
                if (p.kind == Json::Kind::String) m.params.push_back(p.text);
            }
            if (m.name.empty()) continue;
            if (VrcGenerated::IsPropertyAccessor(m.name)) continue;
            // The same overload can be listed more than once, and a capture that
            // distinguishes two methods only by their parameter names would
            // otherwise emit both. IL2CPP dispatch is by name, arity, and types.
            std::string sig = m.name + "(";
            for (const auto &p : m.params) sig += p + ";";
            sig += ")->" + m.return_type;
            if (!entry.method_sigs.insert(sig).second) continue;
            if (!m.return_type.empty() && m.return_type != "System.Void") {
                const Fidelity f = Classify(m.return_type);
                if (f == Fidelity::Unsupported) {
                    unsupported.insert(m.return_type);
                    entry.skipped.push_back("method " + m.name + " -> " + m.return_type);
                    continue;
                }
                if (f == Fidelity::Opaque) opaque.insert(m.return_type);
            }
            entry.methods.push_back(std::move(m));
        }

        entries.push_back(std::move(entry));
    }

    // Two managed types can reduce to the same C++ symbol, for example
    // VRC.SDKBase.VRCStation and VRC.SDK3.Components.VRCStation. The generated
    // globals would collide, so this is reported rather than left to fail the
    // build 340 headers later.
    {
        std::map<std::string, std::vector<std::string>> by_symbol;
        for (const auto &e : entries) by_symbol[e.cpp_name].push_back(e.managed);
        for (const auto &[symbol, names] : by_symbol) {
            if (names.size() < 2) continue;
            std::fprintf(stderr, "  COLLIDES %s <-", symbol.c_str());
            for (const auto &n : names) std::fprintf(stderr, " %s", n.c_str());
            std::fprintf(stderr, "\n");
        }
    }

    std::printf("//=== SECTION: types ===\n");
    for (const auto &entry : entries) {
        if (!entry.fields.empty()) {
            std::printf("inline const VrcGenerated::MemberSpec k%sFields[] = {\n", entry.cpp_name.c_str());
            for (const auto &f : entry.fields)
                std::printf("    { \"%s\", \"%s\", nullptr },\n", Escape(f.name).c_str(),
                            Escape(f.type).c_str());
            std::printf("    { nullptr, nullptr, nullptr },\n};\n\n");
        }

        if (entry.properties.empty()) { std::printf("\n"); }
        else {
            std::printf("inline const VrcGenerated::MemberSpec k%sProperties[] = {\n", entry.cpp_name.c_str());
            for (const auto &p : entry.properties)
                std::printf("    { \"%s\", \"%s\", nullptr },\n", Escape(p.name).c_str(),
                            Escape(p.type).c_str());
            std::printf("    { nullptr, nullptr, nullptr },\n};\n\n");
        }

        for (size_t i = 0; i < entry.methods.size(); ++i) {
            const auto &m = entry.methods[i];
            // Overloads share a name, so the array symbol carries an index. The
            // method name stays real, which is what runtime lookup matches on.
            const std::string symbol = ParamsSymbol(entry, i);
            std::printf("inline const char *const k%s[] = {\n", symbol.c_str());
            for (const auto &p : m.params) std::printf("    \"%s\",\n", Escape(p).c_str());
            std::printf("    nullptr,\n};\n");
        }
        std::printf("\n");
    }

    for (const auto &entry : entries) {
        if (entry.methods.empty()) continue;
        std::printf("inline const VrcGenerated::MethodSpec k%sMethods[] = {\n",
                    entry.cpp_name.c_str());
        for (size_t i = 0; i < entry.methods.size(); ++i) {
            const auto &m = entry.methods[i];
            // A zero-arity method carries no parameter table, matching the
            // hand-written entries.
            std::printf("    { \"%s\", \"%s\", %s, %d, %s },\n", Escape(m.name).c_str(),
                        Escape(m.return_type).c_str(),
                        m.params.empty() ? "nullptr"
                                         : ("k" + ParamsSymbol(entry, i)).c_str(),
                        static_cast<int>(m.params.size()),
                        m.is_static ? "true" : "false");
        }
        std::printf("};\n\n");
    }

    for (const auto &entry : entries) {
        std::printf("// %s -> %s::%s\n", entry.managed.c_str(), entry.cpp_ns.c_str(),
                    entry.cpp_name.c_str());
        std::printf("inline const VrcGenerated::TypeSpec k%s = {\n", entry.cpp_name.c_str());
        std::printf("    \"%s\", \"%s\", \"%s\", \"%s\", \"%s\",\n", entry.cpp_name.c_str(),
                    entry.cpp_ns.c_str(), entry.image.c_str(), entry.ns.c_str(),
                    entry.cls.c_str());
        std::printf("    %s, %s, %s, %s,\n", entry.is_enum ? "true" : "false",
                    entry.is_interface ? "true" : "false",
                    entry.is_static_class ? "true" : "false",
                    entry.is_value_type ? "true" : "false");
        std::printf("    nullptr, 0,\n");
        // An empty array is a zero-size allocation, which /permissive- rejects,
        // and the table already has a nullptr/0 convention for "no members".
        const auto ref = [&](const char *kind, size_t count) {
            std::printf("    %s%s, %d,\n", count == 0 ? "nullptr" : "k",
                        count == 0 ? "" : (entry.cpp_name + kind).c_str(),
                        static_cast<int>(count));
        };
        ref("Fields", entry.fields.size());
        ref("Properties", entry.properties.size());
        ref("Methods", entry.methods.size());
        std::printf("};\n\n");
    }

    if (want_factories) {
        std::printf("//=== SECTION: factories ===\n");
        std::printf("namespace {\n\n");
        for (const auto &entry : entries) {
            std::printf("std::string VRChatGenerated%s() {\n    return VrcGenerated::EmitType(VrcGenerated::k%s, \"%s\");\n}\n\n",
                        entry.cpp_name.c_str(), entry.cpp_name.c_str(),
                        HeaderPath(entry).c_str());
        }
        std::printf("} // namespace\n");
    }

    if (want_writes) {
        std::printf("//=== SECTION: writes ===\n");
        for (const auto &entry : entries) {
            std::printf("    {\"%s\", OutputFilePolicy::GeneratedOverwrite, VRChatGenerated%s(), true, true},\n",
                        HeaderPath(entry).c_str(), entry.cpp_name.c_str());
        }
    }

    if (!opaque.empty()) {
        std::fprintf(stderr, "\nemitted as void* (element type lost; add to ManagedToCpp to fix):\n");
        for (const auto &t : opaque) std::fprintf(stderr, "  %s\n", t.c_str());
    }
    if (!unsupported.empty()) {
        std::fprintf(stderr, "\ndropped, no representation at all:\n");
        for (const auto &t : unsupported) std::fprintf(stderr, "  %s\n", t.c_str());
    }
    for (const auto &entry : entries) {
        for (const auto &s : entry.skipped)
            std::fprintf(stderr, "  skipped %s :: %s\n", entry.managed.c_str(), s.c_str());
    }

    return 0;
}
