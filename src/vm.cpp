#include "vm.h"
#include <cmath>

namespace ask {

// ---------- arithmetic helpers ----------
static bool is_txt_like(const Value& v){ return is_txt(v); }
bool VM::truthy(const Value& v){
    if (is_none(v)) return false;
    if (is_int(v))  return as_int(v)!=0;
    if (is_dec(v))  return as_dec(v)!=0.0;
    if (is_txt(v))  return !as_txt(v).empty();
    if (is_list(v)) return !as_list(v).empty();
    if (is_map(v))  return !as_map(v).empty();
    return false;
}
Value VM::add(const Value& a, const Value& b){
    if (is_txt_like(a) || is_txt_like(b)) return Value{ as_txt(a)+as_txt(b) };
    if (is_dec(a) || is_dec(b)) return Value{ as_dec(a)+as_dec(b) };
    return Value{ as_int(a)+as_int(b) };
}
Value VM::sub(const Value& a, const Value& b){
    if (is_dec(a) || is_dec(b)) return Value{ as_dec(a)-as_dec(b) };
    return Value{ as_int(a)-as_int(b) };
}
Value VM::mul(const Value& a, const Value& b){
    if (is_dec(a) || is_dec(b)) return Value{ as_dec(a)*as_dec(b) };
    return Value{ as_int(a)*as_int(b) };
}
Value VM::div(const Value& a, const Value& b){
    double d = as_dec(b);
    if (d==0.0) throw std::runtime_error("division by zero");
    double res = as_dec(a)/d;
    if (std::floor(res)==res) return Value{ (int64_t)res };
    return Value{ res };
}

// ---------- run ----------
void VM::run(Program* program){
    if (!program) return;

    // 1) collect function definitions
    for (auto& s : program->statements){
        if (s->type == ASTNodeType::Function) {
            auto* f = static_cast<FuncDef*>(s.get());
            Function fn;
            fn.params = f->params;
            fn.body = std::make_unique<Block>(*f->body); // deep copy
            functions[f->name] = std::move(fn);
        }
    }
    // 2) execute top-level non-function statements
    for (auto& s : program->statements){
        if (s->type != ASTNodeType::Function) exec(s.get(), global);
    }
}

// ---------- exec ----------
void VM::exec(Node* n, Env& env){
    switch(n->type){
        case ASTNodeType::Assignment: {
            auto* a = static_cast<Assignment*>(n);
            Value v = eval(a->value.get(), env);
            env.set(a->name, v);
            break;
        }
        case ASTNodeType::PrintStmt: {
            auto* p = static_cast<PrintStmt*>(n);
            Value v = eval(p->value.get(), env);
            rt.callBuiltin("print", {v});
            break;
        }
        case ASTNodeType::If: {
            auto* i = static_cast<If*>(n);
            if (truthy(eval(i->cond.get(), env))) {
                for (auto& st : i->then_block->stmts) exec(st.get(), env);
                break;
            }
            bool done=false;
            for (auto& pr : i->elifs) {
                if (truthy(eval(pr.first.get(), env))) {
                    for (auto& st : pr.second->stmts) exec(st.get(), env);
                    done=true; break;
                }
            }
            if (!done && i->else_block) {
                for (auto& st : i->else_block->stmts) exec(st.get(), env);
            }
            break;
        }
        case ASTNodeType::While: {
            auto* w = static_cast<While*>(n);
            try {
                while (truthy(eval(w->cond.get(), env))) {
                    try {
                        for (auto& st : w->body->stmts) exec(st.get(), env);
                    } catch (const ContSignal&) { continue; }
                }
            } catch (const BreakSignal&) {}
            break;
        }
        case ASTNodeType::For: {
            auto* f = static_cast<For*>(n);
            Value it = eval(f->iterable.get(), env);
            try {
                if (is_list(it)) {
                    auto list = as_list(it);
                    for (const auto& item : list) {
                        env.set(f->var, item);
                        try {
                            for (auto& st : f->body->stmts) exec(st.get(), env);
                        } catch (const ContSignal&) { continue; }
                    }
                } else {
                    throw std::runtime_error("for expects list");
                }
            } catch (const BreakSignal&) {}
            break;
        }
        case ASTNodeType::Return: {
            auto* r = static_cast<Return*>(n);
            Value v = r->value ? eval(r->value.get(), env) : Value{};
            throw ReturnSignal(std::move(v));
        }
        case ASTNodeType::Break:   throw BreakSignal{};
        case ASTNodeType::Continue:throw ContSignal{};
        case ASTNodeType::ExprStmt:{
            auto* e = static_cast<ExprStmt*>(n);
            (void)eval(e->expr.get(), env);
            break;
        }
        default: /* ignore other nodes at stmt level for now */ break;
    }
}

// ---------- eval ----------
Value VM::eval(Node* n, Env& env){
    switch(n->type){
        case ASTNodeType::NumberLiteral: {
            auto* m = static_cast<NumberLiteral*>(n);
            double d = m->value;
            if (std::floor(d)==d) return Value{ (int64_t)d };
            return Value{ d };
        }
        case ASTNodeType::VariableRef: {
            auto* v = static_cast<VariableRef*>(n);
            return env.get(v->name);
        }
        case ASTNodeType::BinaryExpr: {
            auto* b = static_cast<BinaryExpr*>(n);
            Value L = eval(b->left.get(), env);
            Value R = eval(b->right.get(), env);
            if (b->op=="+") return add(L,R);
            if (b->op=="-") return sub(L,R);
            if (b->op=="*") return mul(L,R);
            if (b->op=="/") return div(L,R);
            if (b->op=="==") return Value{ (int64_t)(as_txt(L)==as_txt(R)) };
            if (b->op=="!=") return Value{ (int64_t)(as_txt(L)!=as_txt(R)) };
            if (b->op=="<")  return Value{ (int64_t)(as_dec(L)< as_dec(R)) };
            if (b->op=="<=") return Value{ (int64_t)(as_dec(L)<=as_dec(R)) };
            if (b->op==">")  return Value{ (int64_t)(as_dec(L)> as_dec(R)) };
            if (b->op==">=") return Value{ (int64_t)(as_dec(L)>=as_dec(R)) };
            throw std::runtime_error("unknown binary op: "+b->op);
        }
        case ASTNodeType::UnaryExpr: {
            auto* u = static_cast<UnaryOp*>(n);
            Value x = eval(u->expr.get(), env);
            if (u->op=="-") return Value{ -as_dec(x) };
            if (u->op=="not") return Value{ (int64_t)!truthy(x) };
            throw std::runtime_error("unknown unary op");
        }
        case ASTNodeType::Call: {
            auto* c = static_cast<Call*>(n);
            // Only simple name calls for now
            if (c->func->type != ASTNodeType::VariableRef)
                throw std::runtime_error("only simple calls supported");
            std::string name = static_cast<VariableRef*>(c->func.get())->name;
            std::vector<Value> args;
            args.reserve(c->args.size());
            for (auto& a : c->args) args.emplace_back(eval(a.get(), env));
            return callFunction(name, args, env);
        }
        case ASTNodeType::ListLiteral: {
            auto* l = static_cast<ListLiteral*>(n);
            List out; out.reserve(l->items.size());
            for (auto& it : l->items) out.emplace_back(eval(it.get(), env));
            return Value{ out };
        }
        case ASTNodeType::MapLiteral: {
            auto* m = static_cast<MapLiteral*>(n);
            Map out;
            for (auto& kv : m->pairs) {
                auto k = kv.first; // already a txt
                out[k] = eval(kv.second.get(), env);
            }
            return Value{ out };
        }
        case ASTNodeType::Index: {
            auto* idx = static_cast<Index*>(n);
            Value c = eval(idx->seq.get(), env);
            Value i = eval(idx->idx.get(), env);
            if (is_list(c)) {
                auto& lst = as_list(c);
                int64_t pos = as_int(i);
                if (pos<0 || (size_t)pos>=lst.size()) throw std::runtime_error("list index OOB");
                return lst[(size_t)pos];
            } else if (is_map(c)) {
                auto& mp = as_map(c);
                auto it = mp.find(as_txt(i));
                if (it==mp.end()) throw std::runtime_error("key not found");
                return it->second;
            }
            throw std::runtime_error("index on non-container");
        }
        default: throw std::runtime_error("unsupported expression node");
    }
}

// ---------- call ----------
Value VM::callFunction(const std::string& name, const std::vector<Value>& args, Env& env){
    // 1) builtins
    if (functions.find(name)==functions.end()) {
        return rt.callBuiltin(name, args);
    }
    // 2) user-defined
    auto& fn = functions.at(name);
    if (args.size()!=fn.params.size())
        throw std::runtime_error("arity mismatch in call to "+name);
    Env callEnv(&global); // simple global parent; capture env for closures later
    for (size_t i=0;i<args.size();++i) callEnv.table[fn.params[i]] = args[i];
    try {
        for (auto& st : fn.body->stmts) exec(st.get(), callEnv);
    } catch (const ReturnSignal& rs) {
        return rs.v;
    }
    return {}; // None
}

} // namespace ask
