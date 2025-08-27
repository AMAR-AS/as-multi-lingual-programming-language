#pragma once
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>

namespace ask {

// Value model: none | int | dec | txt | list | map
struct Value;
using List = std::vector<Value>;
using Map  = std::unordered_map<std::string, Value>;

using ValueVariant = std::variant<std::monostate, int64_t, double, std::string, List, Map>;

struct Value {
    ValueVariant v;
    Value() = default;
    template<class T> Value(T x) : v(std::move(x)) {}
};

inline bool is_none(const Value& x)   { return std::holds_alternative<std::monostate>(x.v); }
inline bool is_int(const Value& x)    { return std::holds_alternative<int64_t>(x.v); }
inline bool is_dec(const Value& x)    { return std::holds_alternative<double>(x.v); }
inline bool is_txt(const Value& x)    { return std::holds_alternative<std::string>(x.v); }
inline bool is_list(const Value& x)   { return std::holds_alternative<List>(x.v); }
inline bool is_map(const Value& x)    { return std::holds_alternative<Map>(x.v); }

int64_t  as_int(const Value& x);
double   as_dec(const Value& x);
std::string as_txt(const Value& x);
List&    as_list(Value& x);
const List& as_list(const Value& x);
Map&     as_map(Value& x);
const Map& as_map(const Value& x);

// Builtin function signature: returns Value, receives argv
using Builtin = std::function<Value(const std::vector<Value>&)>;

class Runtime {
public:
    Runtime();

    void setVar(const std::string& name, Value v) { vars[name] = std::move(v); }
    bool hasVar(const std::string& name) const    { return vars.find(name)!=vars.end(); }
    Value getVar(const std::string& name) const;

    void registerBuiltin(const std::string& name, Builtin fn);
    Value callBuiltin(const std::string& name, const std::vector<Value>& args) const;

private:
    std::unordered_map<std::string, Value> vars;
    std::unordered_map<std::string, Builtin> builtins;
};

} // namespace ask
