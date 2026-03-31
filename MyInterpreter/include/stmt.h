#pragma once
#include "expr.h"
#include<memory>

struct ExpressionStmt;
struct PrintStmt;
struct VarStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;

struct StmtVisitor{
    virtual void visitExpressionStmt(ExpressionStmt* stmt) = 0;
    virtual void visitPrintStmt(PrintStmt* stmt) = 0;
    virtual void visitVarStmt(VarStmt* stmt) = 0;
    virtual void visitBlockStmt(BlockStmt* stmt) = 0;
    virtual void visitIfStmt(IfStmt* stmt) = 0;
    virtual void visitWhileStmt(WhileStmt* stmt) = 0;
    virtual ~StmtVisitor() = default;
};

struct Stmt{
    virtual void accept(StmtVisitor* visitor) = 0;
    virtual ~Stmt() = default;
};

struct ExpressionStmt : Stmt{
    std::unique_ptr<Expr> expression;

    ExpressionStmt(std::unique_ptr<Expr> expression) :
        expression(std::move(expression)) {}

    void accept(StmtVisitor* visitor) override{
        visitor->visitExpressionStmt(this);
    }
};

struct PrintStmt : Stmt{
    std::unique_ptr<Expr> expression;

    PrintStmt(std::unique_ptr<Expr> expression):
        expression(std::move(expression)) {}

    void accept(StmtVisitor* visitor) override{
        visitor->visitPrintStmt(this);
    }
};

struct VarStmt : Stmt{
    Token name;
    std::unique_ptr<Expr> initalizer;

    VarStmt(Token name, std::unique_ptr<Expr> initalizer) :
        name(std::move(name)), initalizer(std::move(initalizer)) {}

    void accept(StmtVisitor* visitor) override{
        visitor->visitVarStmt(this);
    }
};

struct BlockStmt : Stmt{
    std::vector<std::unique_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : 
        statements(std::move(statements)) {}

    void accept(StmtVisitor* visitor) override{
        visitor->visitBlockStmt(this);
    }
};

struct IfStmt : Stmt{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch) : 
        condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)){}

    void accept(StmtVisitor* visitor) override{
        visitor->visitIfStmt(this);
    }
};

struct WhileStmt : Stmt{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body) :
        condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor* visitor) override{
        visitor->visitWhileStmt(this);
    }
};