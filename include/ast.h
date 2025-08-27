#pragma once
#include <string>
#include <vector>
#include <memory>

namespace ask {

enum class ASTNodeType {
    Program,
    NumberLiteral,
    VariableRef,
    BinaryExpr,
    Assignment,
    PrintStmt
};

struct ASTNode {
    ASTNodeType type;
    virtual ~ASTNode() = default;
};

using ASTPtr = std::unique_ptr<ASTNode>;

struct NumberLiteral : ASTNode {
    double value;
    NumberLiteral(double v) : value(v) { type = ASTNodeType::NumberLiteral; }
};

struct VariableRef : ASTNode {
    std::string name;
    VariableRef(std::string n) : name(std::move(n)) { type = ASTNodeType::VariableRef; }
};

struct BinaryExpr : ASTNode {
    std::string op;
    ASTPtr left, right;
    BinaryExpr(std::string o, ASTPtr l, ASTPtr r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {
        type = ASTNodeType::BinaryExpr;
    }
};

struct Assignment : ASTNode {
    std::string name;
    ASTPtr value;
    Assignment(std::string n, ASTPtr v)
        : name(std::move(n)), value(std::move(v)) { type = ASTNodeType::Assignment; }
};

struct PrintStmt : ASTNode {
    ASTPtr value;
    PrintStmt(ASTPtr v) : value(std::move(v)) { type = ASTNodeType::PrintStmt; }
};

struct Program : ASTNode {
    std::vector<ASTPtr> statements;
    Program() { type = ASTNodeType::Program; }
};

} // namespace ask
