#include "runtime.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <random>

namespace ask {

int64_t as_int(const Value& x) {
    if (is_int(x)) return std::get<int64_t>(x.v);
    if (is_dec(x)) return (int64_t)std::get<double>(x.v);
    if (is_txt(x)) return std::stoll(std::get<std::string>(x.v));
    throw std::runtime_error("expected integer");
}
double as_dec(const Value& x) {
    if (is_dec(x)) return std::get<double>(x.v);
    if (is_int(x)) return (double)std::get<int64_t>(x.v);
    if (is_txt(x)) return std::stod(std::get<std::string>(x.v));
    throw std::runtime_error("expected number");
}
std::string as_txt(const Value& x) {
    if (is_txt(x)) return std::get<std::string>(x.v);
    if (is_int(x)) return std::to_string(std::get<int64_t>(x.v));
    if (is_dec(x)) { std::ostringstream ss; ss<<std::get<double>(x.v); return ss.str(); }
    if (is_list(x)) {
        const auto& lst=std::get<List>(x.v);
        std::ostringstream ss; ss<<"[";
        for (size_t i=0;i<lst.size();++i){ if(i) ss<<", "; ss<<as_txt(lst[i]); }
        ss<<"]"; return ss.str();
    }
    if (is_map(x)) {
        const auto& mp=std::get<Map>(x.v);
        std::ostringstream ss; ss<<"{";
        size_t i=0; for(auto& kv: mp){ if(i++) ss<<", "; ss<<kv.first<<":"<<as_txt(kv.second); }
        ss<<"}"; return ss.str();
    }
    return "None";
}
List& as_list(Value& x) {
    if (!is_list(x)) throw std::runtime_error("expected list");
    return std::get<List>(x.v);
}
const List& as_list(const Value& x) {
    if (!is_list(x)) throw std::runtime_error("expected list");
    return std::get<List>(x.v);
}
Map& as_map(Value& x) {
    if (!is_map(x)) throw std::runtime_error("expected map");
    return std::get<Map>(x.v);
}
const Map& as_map(const Value& x) {
    if (!is_map(x)) throw std::runtime_error("expected map");
    return std::get<Map>(x.v);
}

Value Runtime::getVar(const std::string& name) const {
    auto it = vars.find(name);
    if (it == vars.end()) throw std::runtime_error("undefined var: " + name);
    return it->second;
}

void Runtime::registerBuiltin(const std::string& name, Builtin fn) {
    builtins[name] = std::move(fn);
}
Value Runtime::callBuiltin(const std::string& name, const std::vector<Value>& args) const {
    auto it = builtins.find(name);
    if (it == builtins.end()) throw std::runtime_error("unknown builtin: " + name);
    return it->second(args);
}

Runtime::Runtime() {
    // ---- core I/O ----
    registerBuiltin("print", [](const std::vector<Value>& a)->Value{
        for (size_t i=0;i<a.size();++i){ if(i) std::cout<<" "; std::cout<<as_txt(a[i]); }
        std::cout<<"\n"; return {};
    });
    registerBuiltin("input", [](const std::vector<Value>& a)->Value{
        if (!a.empty()) std::cout<<as_txt(a[0]);
        std::string s; std::getline(std::cin, s); return Value{s};
    });

    // ---- conversions ----
    registerBuiltin("int", [](const std::vector<Value>& a)->Value{ return Value{as_int(a.at(0))}; });
    registerBuiltin("dec", [](const std::vector<Value>& a)->Value{ return Value{as_dec(a.at(0))}; });
    registerBuiltin("txt", [](const std::vector<Value>& a)->Value{ return Value{as_txt(a.at(0))}; });

    // ---- math ----
    registerBuiltin("sqrt", [](const std::vector<Value>& a)->Value{
        double x = as_dec(a.at(0)); if (x<0) throw std::runtime_error("sqrt of negative");
        return Value{ std::sqrt(x) };
    });
    registerBuiltin("pow", [](const std::vector<Value>& a)->Value{
        return Value{ std::pow(as_dec(a.at(0)), as_dec(a.at(1))) };
    });

    // ---- strings / collections ----
    registerBuiltin("len", [](const std::vector<Value>& a)->Value{
        const Value& x=a.at(0);
        if (is_txt(x))  return Value{ (int64_t)std::get<std::string>(x.v).size() };
        if (is_list(x)) return Value{ (int64_t)std::get<List>(x.v).size() };
        if (is_map(x))  return Value{ (int64_t)std::get<Map>(x.v).size() };
        throw std::runtime_error("len() unsupported");
    });
    registerBuiltin("split", [](const std::vector<Value>& a)->Value{
        std::string s = as_txt(a.at(0)), sep = as_txt(a.at(1));
        List out; size_t pos=0, f; 
        while ((f=s.find(sep,pos))!=std::string::npos){ out.emplace_back(s.substr(pos,f-pos)); pos=f+sep.size(); }
        out.emplace_back(s.substr(pos));
        return Value{ out };
    });
    registerBuiltin("join", [](const std::vector<Value>& a)->Value{
        std::string sep = as_txt(a.at(1));
        const List& lst = std::get<List>(a.at(0).v);
        std::ostringstream ss;
        for (size_t i=0;i<lst.size();++i){ if(i) ss<<sep; ss<<as_txt(lst[i]); }
        return Value{ ss.str() };
    });

    // ---- arrays ----
    registerBuiltin("array", [](const std::vector<Value>& a)->Value{
        int64_t n = as_int(a.at(0)); Value init = (a.size()>1)?a[1]:Value{};
        List lst; lst.resize((size_t)n, init); return Value{ lst };
    });

    // ---- time/random ----
    registerBuiltin("now_ms", [](const std::vector<Value>&)->Value{
        using namespace std::chrono;
        auto ms = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
        return Value{ (int64_t)ms };
    });
    registerBuiltin("randint", [](const std::vector<Value>& a)->Value{
        static std::mt19937_64 rng{std::random_device{}()};
        std::uniform_int_distribution<int64_t> dist(as_int(a.at(0)), as_int(a.at(1)));
        return Value{ dist(rng) };
    });
}

} // namespace ask
