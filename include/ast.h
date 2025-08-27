#pragma once
#include <string>
#include <vector>

struct ASTNode {
    std::string nodeType;
    std::string value;
    std::vector<ASTNode*> children;

    ASTNode(const std::string& type, const std::string& val = "") 
        : nodeType(type), value(val) {}
};
struct PrintStmt : public Stmt {
    Expr* value;
    PrintStmt(Expr* v) : value(v) {}
    void accept(ASTVisitor& v) override { v.visitPrint(this); }
};
