#include "parser.h"
#include <sstream>
#include <unordered_map>

namespace ask {

Parser::Parser(const std::vector<Token>& tokens) : toks(tokens), i(0) {}

const Token& Parser::peek(int k) const {
    size_t j = i + (size_t)k;
    if (j >= toks.size()) return toks.back();
    return toks[j];
}
bool Parser::atEnd() const { return peek().type == TokenType::EndOfFile; }
const Token& Parser::advance() { if (!atEnd()) i++; return peek(-1 + 0); } // not used
bool Parser::match(const std::string& lexeme) {
    if (peek().lexeme == lexeme) { i++; return true; }
    return false;
}
bool Parser::matchType(TokenType t) {
    if (peek().type == t) { i++; return true; }
    return false;
}
bool Parser::check(const std::string& lex) const { return peek().lexeme == lex; }
bool Parser::checkType(TokenType t) const { return peek().type == t; }

bool Parser::consume(const std::string& lex, const char* msg) {
    if (match(lex)) return true;
    std::ostringstream os;
    os << "Parse error at line " << peek().line << ":" << peek().column
       << " — expected '" << lex << "'; " << msg << ", found '" << peek().lexeme << "'";
    errs.push_back(os.str());
    return false;
}

int Parser::precedenceOf(const std::string& op) const {
    static const std::unordered_map<std::string,int> P = {
        {"||",1},
        {"&&",2},
        {"==",3},{"!=",3},
        {"<",4},{"<=",4},{">",4},{">=",4},
        {"+",5},{"-",5},
        {"*",6},{"/",6},{"%",6},
        {".",9},{"(",9},{"[",9},{"++",9},{"--",9}
    };
    auto it = P.find(op);
    return it==P.end() ? -1 : it->second;
}

void Parser::sync() {
    // panic until newline, semicolon, or block boundary or statement-start keyword
    while (!atEnd()) {
        if (peek().lexeme == ";" || peek().type == TokenType::Newline ||
            peek().lexeme == "}" ) { i++; break; }
        if (peek().type == TokenType::Keyword) break;
        i++;
    }
}

// ---- Top-level ----
std::shared_ptr<Program> Parser::parse() {
    auto prog = std::make_shared<Program>();
    while (!atEnd()) {
        auto st = statement();
        if (st) prog->stmts.push_back(st);
        else sync();
        // skip optional newlines/semicolons
        while (peek().type == TokenType::Newline || peek().lexeme == ";") i++;
    }
    return prog;
}

StmtPtr Parser::statement() {
    const Token& t = peek();
    if (t.type == TokenType::Keyword) {
        const std::string& k = t.lexeme;
        if (k == "if" || k == "check") return ifStmt();
        if (k == "while" || k == "when") return whileStmt();
        if (k == "for" || k == "let") return forStmt();
        if (k == "fcn" || k == "function") return funcDef();
        if (k == "ret" || k == "giveback") return returnStmt();
        if (k == "break" || k == "brk" || k == "exit") return breakStmt();
        if (k == "continue" || k == "cont" || k == "skip") return continueStmt();
        if (k == "try" || k == "hit") return tryCatchStmt();
        if (k == "with") return withStmt();
    }
    return varAssignOrExprStmt();
}

StmtPtr Parser::varAssignOrExprStmt() {
    if (peek().type == TokenType::Identifier && peek(1).lexeme == "=") {
        std::string name = peek().lexeme;
        i += 2; // consume IDENT '='
        auto val = expression();
        auto n = std::make_shared<Assign>();
        n->name = std::move(name);
        n->value = val;
        return n;
    }
    auto e = expression();
    auto s = std::make_shared<ExprStmt>();
    s->expr = e;
    return s;
}

std::shared_ptr<Block> Parser::block() {
    auto b = std::make_shared<Block>();
    if (match("{")) {
        while (!atEnd() && !match("}")) {
            b->stmts.push_back(statement());
            while (peek().type == TokenType::Newline || peek().lexeme == ";") i++;
        }
        return b;
    }
    // single-line statement fallback
    b->stmts.push_back(statement());
    return b;
}

StmtPtr Parser::ifStmt() {
    i++; // consume if/check
    auto cond = expression();
    auto thenB = block();
    auto node = std::make_shared<If>();
    node->arms.push_back({cond, thenB});

    while (peek().lexeme == "elif" || peek().lexeme == "orif") {
        i++;
        auto c = expression();
        auto b = block();
        node->arms.push_back({c, b});
    }
    if (peek().lexeme == "else") {
        i++;
        node->elseBlock = block();
    }
    return node;
}

StmtPtr Parser::whileStmt() {
    i++; // while/when
    auto cond = expression();
    auto body = block();
    auto w = std::make_shared<While>();
    w->cond = cond; w->body = body;
    return w;
}

StmtPtr Parser::forStmt() {
    i++; // for/let
    // for x in iterable { ... }
    if (peek().type != TokenType::Identifier) {
        errs.push_back("for/let expects loop variable");
        return nullptr;
    }
    std::string var = peek().lexeme; i++;
    if (!consume("in", "expected 'in' after loop variable")) return nullptr;
    auto iter = expression();
    auto body = block();
    auto f = std::make_shared<ForIn>();
    f->var = var; f->iter = iter; f->body = body;
    return f;
}

StmtPtr Parser::funcDef() {
    i++; // fcn/function
    if (peek().type != TokenType::Identifier) {
        errs.push_back("function name expected");
        return nullptr;
    }
    std::string name = peek().lexeme; i++;
    if (!consume("(", "expected '(' after function name")) return nullptr;

    std::vector<Param> params;
    if (!check(")")) {
        while (true) {
            if (peek().type != TokenType::Identifier) { errs.push_back("parameter name expected"); break; }
            Param p; p.name = peek().lexeme; i++;
            if (match(":") && peek().type == TokenType::Identifier) { p.type = peek().lexeme; i++; }
            params.push_back(std::move(p));
            if (!match(",")) break;
        }
    }
    if (!consume(")", "expected ')' after parameters")) return nullptr;

    std::optional<std::string> retType;
    if (match("->") && peek().type == TokenType::Identifier) { retType = peek().lexeme; i++; }

    auto body = block();
    auto fn = std::make_shared<FuncDef>();
    fn->name = std::move(name); fn->params = std::move(params); fn->retType = retType; fn->body = body;
    return fn;
}

StmtPtr Parser::returnStmt() {
    i++; // ret/giveback
    if (peek().type == TokenType::Newline || peek().lexeme == "}" || peek().lexeme == ";") {
        auto r = std::make_shared<Return>();
        r->value = std::nullopt;
        return r;
    }
    auto val = expression();
    auto r = std::make_shared<Return>();
    r->value = val;
    return r;
}

StmtPtr Parser::breakStmt() { i++; return std::make_shared<Break>(); }
StmtPtr Parser::continueStmt() { i++; return std::make_shared<Continue>(); }

StmtPtr Parser::tryCatchStmt() {
    i++; // try/hit
    auto tb = block();
    if (!consume("catch", "expected 'catch'")) return nullptr;
    if (!consume("(", "expected '(' after catch")) return nullptr;
    if (peek().type != TokenType::Identifier) { errs.push_back("catch requires identifier"); return nullptr; }
    std::string errName = peek().lexeme; i++;
    if (!consume(")", "expected ')' after catch identifier")) return nullptr;
    auto cb = block();
    std::shared_ptr<Block> fb = nullptr;
    if (match("finally") || match("cleanup")) fb = block();
    auto tc = std::make_shared<TryCatch>();
    tc->tryBlock = tb; tc->errName = errName; tc->catchBlock = cb; tc->finallyBlock = fb;
    return tc;
}

StmtPtr Parser::withStmt() {
    i++; // with
    auto ctx = expression();
    auto body = block();
    auto w = std::make_shared<With>();
    w->ctx = ctx; w->body = body;
    return w;
}

// ---- Expressions (Pratt) ----
ExprPtr Parser::expression() {
    // precedence climbing from unary
    auto left = unary();
    // handle binary with precedence
    while (true) {
        std::string op = peek().lexeme;
        int prec = precedenceOf(op);
        if (prec < 1 || prec > 6) break; // only binary levels
        i++; // consume op
        auto right = unary();
        // combine with precedence/associativity (left-assoc)
        while (true) {
            std::string nextOp = peek().lexeme;
            int nextPrec = precedenceOf(nextOp);
            if (nextPrec > prec && nextPrec <= 6) {
                i++; // consume nested op
                auto rhs2 = unary();
                auto bin2 = std::make_shared<Binary>();
                bin2->op = nextOp; bin2->left = right; bin2->right = rhs2;
                right = bin2;
            } else break;
        }
        auto bin = std::make_shared<Binary>();
        bin->op = op; bin->left = left; bin->right = right;
        left = bin;
    }
    // postfix chain
    left = postfix(left);
    return left;
}

ExprPtr Parser::unary() {
    const std::string op = peek().lexeme;
    if (op == "!" || op == "-" || op == "+") {
        i++;
        auto e = unary();
        auto u = std::make_shared<Unary>(); u->op = op; u->expr = e;
        return u;
    }
    auto p = primary();
    // allow postfix directly after primary too
    return postfix(p);
}

ExprPtr Parser::postfix(ExprPtr prim) {
    auto node = prim;
    for (;;) {
        if (match("(")) {
            std::vector<ExprPtr> args;
            if (!check(")")) {
                while (true) {
                    args.push_back(expression());
                    if (!match(",")) break;
                }
            }
            consume(")", "expected ')' after arguments");
            auto call = std::make_shared<Call>(); call->callee = node; call->args = std::move(args);
            node = call; continue;
        }
        if (match("[")) {
            auto idx = expression();
            consume("]", "expected ']' after index");
            auto ix = std::make_shared<Index>(); ix->target = node; ix->index = idx;
            node = ix; continue;
        }
        if (match(".")) {
            if (peek().type != TokenType::Identifier) { errs.push_back("member name expected after '.'"); break; }
            auto mem = std::make_shared<Member>(); mem->target = node; mem->name = peek().lexeme; i++;
            node = mem; continue;
        }
        if (match("++") || match("--")) {
            auto pf = std::make_shared<PostFix>(); pf->op = toks[i-1].lexeme; pf->target = node;
            node = pf; continue;
        }
        break;
    }
    return node;
}

ExprPtr Parser::primary() {
    const Token& t = peek();
    if (t.type == TokenType::Identifier) { i++; return std::make_shared<Ident>(t.lexeme); }
    if (t.type == TokenType::Number)     { i++; return std::make_shared<Literal>(Literal::Number, t.lexeme); }
    if (t.type == TokenType::String)     { i++; return std::make_shared<Literal>(Literal::String, t.lexeme); }
    if (t.type == TokenType::Bool)       { i++; return std::make_shared<Literal>(Literal::Bool, t.lexeme); }
    if (t.type == TokenType::NoneLiteral){ i++; return std::make_shared<Literal>(Literal::None, t.lexeme); }
    if (match("(")) {
        auto e = expression();
        consume(")", "expected ')' after group");
        return e;
    }
    errs.push_back("unexpected token in expression: '" + t.lexeme + "'");
    // return dummy to continue
    i++;
    return std::make_shared<Literal>(Literal::None, "none");
}

} // namespace ask
