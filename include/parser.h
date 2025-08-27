#ifndef ASK_PARSER_H
#define ASK_PARSER_H

#include "lexer.h"
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace ask {

// ---- AST ----
struct Node {
    virtual ~Node() = default;
};
using NodePtr = std::shared_ptr<Node>;

struct Expr : Node {};
using ExprPtr = std::shared_ptr<Expr>;
struct Stmt : Node {};
using StmtPtr = std::shared_ptr<Stmt>;

struct Program : Node { std::vector<StmtPtr> stmts; };

struct Ident : Expr { std::string name; explicit Ident(std::string n):name(std::move(n)){} };
struct Literal : Expr {
    enum Kind { Number, String, Bool, None } kind;
    std::string value; // raw
    Literal(Kind k, std::string v):kind(k),value(std::move(v)){}
};

struct Unary : Expr { std::string op; ExprPtr expr; };
struct Binary : Expr { std::string op; ExprPtr left, right; };
struct Call : Expr { ExprPtr callee; std::vector<ExprPtr> args; };
struct Index : Expr { ExprPtr target; ExprPtr index; };
struct Member : Expr { ExprPtr target; std::string name; };
struct PostFix : Expr { std::string op; ExprPtr target; };

struct Assign : Stmt { std::string name; ExprPtr value; };
struct ExprStmt : Stmt { ExprPtr expr; };
struct Return : Stmt { std::optional<ExprPtr> value; };
struct Break : Stmt {};
struct Continue : Stmt {};

struct Block : Stmt { std::vector<StmtPtr> stmts; };

struct If : Stmt {
    std::vector<std::pair<ExprPtr, std::shared_ptr<Block>>> arms;
    std::shared_ptr<Block> elseBlock; // nullable
};

struct While : Stmt { ExprPtr cond; std::shared_ptr<Block> body; };

struct ForIn : Stmt { std::string var; ExprPtr iter; std::shared_ptr<Block> body; };

struct Param { std::string name; std::optional<std::string> type; };
struct FuncDef : Stmt {
    std::string name;
    std::vector<Param> params;
    std::optional<std::string> retType;
    std::shared_ptr<Block> body;
};

struct TryCatch : Stmt {
    std::shared_ptr<Block> tryBlock;
    std::string errName;
    std::shared_ptr<Block> catchBlock;
    std::shared_ptr<Block> finallyBlock; // nullable
};

struct With : Stmt { ExprPtr ctx; std::shared_ptr<Block> body; };

// ---- Parser ----
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::shared_ptr<Program> parse();

    const std::vector<std::string>& errors() const { return errs; }

private:
    const std::vector<Token>& toks;
    size_t i;
    std::vector<std::string> errs;

    const Token& peek(int k = 0) const;
    bool atEnd() const;
    bool match(const std::string& lexeme);
    bool matchType(TokenType t);
    bool check(const std::string& lexeme) const;
    bool checkType(TokenType t) const;
    const Token& advance();
    bool consume(const std::string& lexeme, const char* msg);

    // statements
    StmtPtr statement();
    StmtPtr varAssignOrExprStmt();
    std::shared_ptr<Block> block();
    StmtPtr ifStmt();
    StmtPtr whileStmt();
    StmtPtr forStmt();
    StmtPtr funcDef();
    StmtPtr returnStmt();
    StmtPtr breakStmt();
    StmtPtr continueStmt();
    StmtPtr tryCatchStmt();
    StmtPtr withStmt();

    // expressions (Pratt)
    ExprPtr expression();
    ExprPtr parsePrecedence(int minPrec);
    ExprPtr unary();
    ExprPtr postfix(ExprPtr prim);
    ExprPtr primary();

    int precedenceOf(const std::string& op) const;

    void sync(); // error recovery
};

} // namespace ask

#endif
