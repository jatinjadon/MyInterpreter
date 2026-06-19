#pragma once
#include "expr.h"
#include "stmt.h"
#include<vector>
#include<unordered_map>
#include "Interpreter.h"

class Resolver : public ExprVisitor, public StmtVisitor {
    // these two enums are used to check if return statement is valid
    enum class FunctionType {
        NONE,
        FUNCTION
    };
    enum class ClassType {
        NONE,
        CLASS
    };
    Interpreter* interpreter;
    std::vector<std::unordered_map<std::string, bool>> scopes;
    std::unordered_map<std::string, bool> globalScope; // store the global scope, to check if a variable is declared in global scope
    void beginScope();
    void endScope();
    void declare(const Token& name);
    void define(const Token& name);
    void resolve(Stmt* stmt);
    void resolve(Expr* expr);
    void resolveLocal(Expr* expr, const Token& name); 
    // resolve a variable, find the distance between current scope and the scope where the variable is declared,
    // then store the distance in interpreter for later use

    void visitBlockStmt(BlockStmt* stmt) override;
    void visitVarStmt(VarStmt* stmt) override;
    void visitFunctionStmt(FunctionStmt* stmt) override;
    void visitExpressionStmt(ExpressionStmt* stmt) override;
    void visitIfStmt(IfStmt* stmt) override;
    void visitPrintStmt(PrintStmt* stmt) override;
    void visitReturnStmt(ReturnStmt* stmt) override;
    void visitWhileStmt(WhileStmt* stmt) override;

    LoxValue visitBinaryExpr(Binary* expr) override;
    LoxValue visitUnaryExpr(Unary* expr) override;
    LoxValue visitLiteralExpr(Literal* expr) override;
    LoxValue visitGroupingExpr(Grouping* expr) override;
    LoxValue visitVariableExpr(Variable* expr) override;
    LoxValue visitAssignExpr(Assign* expr) override;
    LoxValue visitLogicalExpr(Logical* expr) override;
    LoxValue visitCallExpr(Call* expr) override;
    
    void resolveFunction(FunctionStmt* function, FunctionType type); 
    // update currentFunction when resolving a function, to check if return statement is valid

    void error(const Token& token, const std::string& message);
    ClassType currentClass = ClassType::NONE;
    FunctionType currentFunction = FunctionType::NONE;

public:
    bool hadError = false;
    Resolver(Interpreter* interpreter) : interpreter(interpreter) {}
    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);
};