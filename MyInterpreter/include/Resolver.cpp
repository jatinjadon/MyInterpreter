#include "Resolver.h"

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        resolve(stmt.get());
    }
}
void Resolver::resolve(Stmt* stmt) {
    stmt->accept(this);
}
void Resolver::resolve(Expr* expr) {
    expr->accept(this);
}
void Resolver::resolveLocal(Expr* expr, const Token& name) {
    int distance = 0;
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it, ++distance) {
        if (it->find(name.lexeme) != it->end()) {
            interpreter->resolve(expr, distance);
            return;
        }
    }
    // not found, assume it is global
}
void Resolver::declare(const Token& name) {
    if (scopes.empty()) {
        // If there are no scopes, we are in the global scope.
        if (globalScope.find(name.lexeme) != globalScope.end()) {
            error(name, "Variable with this name already declared in the global scope.");
        }
        globalScope[name.lexeme] = false; 
        return;
    }
    auto& scope = scopes.back();
    if (scope.find(name.lexeme) != scope.end()) {
        error(name, "Variable with this name already declared in this scope.");
    }
    scope[name.lexeme] = false; // false means the variable is declared but not defined yet
}
void Resolver::define(const Token& name) {
    if (scopes.empty()) {
        globalScope[name.lexeme] = true;
        return;
    }
    scopes.back()[name.lexeme] = true; // true means the variable is defined
}
void Resolver::beginScope() {
    scopes.emplace_back();
}
void Resolver::endScope() {
    scopes.pop_back();
}
void Resolver::error(const Token& token, const std::string& message) {
    hadError = true;
    if (token.type == _EOF) {
        std::cerr << "[line " << token.line << "] Error at end: " << message << std::endl;
    } else {
        std::cerr << "[line " << token.line << "] Error at '" << token.lexeme << "': " << message << std::endl;
    }
}
void Resolver::resolveFunction(FunctionStmt* function, FunctionType type) {
    // Save the previous state before we overwrite it. 
    // (Crucial for when a function is declared inside another function!)
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (const Token& param : function->params) {
        declare(param);
        define(param);
    }
    resolve(function->body);
    endScope();

    currentFunction = enclosingFunction;
}
void Resolver::visitBlockStmt(BlockStmt* stmt) {
    beginScope();
    resolve(stmt->statements);
    endScope();
}
void Resolver::visitVarStmt(VarStmt* stmt) {
    declare(stmt->name);
    if (stmt->initalizer) {
        resolve(stmt->initalizer.get());
    }
    define(stmt->name);
}
void Resolver::visitFunctionStmt(FunctionStmt* stmt) {
    declare(stmt->name);
    define(stmt->name);
    resolveFunction(stmt, FunctionType::FUNCTION);
}
void Resolver::visitExpressionStmt(ExpressionStmt* stmt) {
    resolve(stmt->expression.get());
}
void Resolver::visitIfStmt(IfStmt* stmt) {
    resolve(stmt->condition.get());
    resolve(stmt->thenBranch.get());
    if (stmt->elseBranch) {
        resolve(stmt->elseBranch.get());
    }
}
void Resolver::visitPrintStmt(PrintStmt* stmt) {
    resolve(stmt->expression.get());
}
void Resolver::visitReturnStmt(ReturnStmt* stmt) {
    if (currentFunction == FunctionType::NONE) {
        error(stmt->keyword, "Can't return from top-level code.");
    }
    if (stmt->value) {
        resolve(stmt->value.get());
    }
}
void Resolver::visitWhileStmt(WhileStmt* stmt) {
    resolve(stmt->condition.get());
    resolve(stmt->body.get());
}
LoxValue Resolver::visitBinaryExpr(Binary* expr) {
    resolve(expr->left.get());
    resolve(expr->right.get());
    return nullptr;
}
LoxValue Resolver::visitUnaryExpr(Unary* expr) {
    resolve(expr->right.get());
    return nullptr;
}
LoxValue Resolver::visitLiteralExpr(Literal* expr) {
    return nullptr;
}
LoxValue Resolver::visitGroupingExpr(Grouping* expr) {
    resolve(expr->expr.get());
    return nullptr;
}
LoxValue Resolver::visitVariableExpr(Variable* expr) {
    if (!scopes.empty()) {
        auto& scope = scopes.back();
        auto it = scope.find(expr->name.lexeme);
        if (it != scope.end() && it->second == false) {
            error(expr->name, "Can't read local variable in its own initializer.");
        }
    }
    else{
        auto it = globalScope.find(expr->name.lexeme);
        if (it != globalScope.end() && it->second == false) {
            error(expr->name, "Can't read global variable in its own initializer.");
        }
    }
    resolveLocal(expr, expr->name);
    return nullptr;
}
LoxValue Resolver::visitAssignExpr(Assign* expr) {
    resolve(expr->expr.get());
    resolveLocal(expr, expr->name);
    return nullptr;
}
LoxValue Resolver::visitLogicalExpr(Logical* expr) {
    resolve(expr->left.get());
    resolve(expr->right.get());
    return nullptr;
}
LoxValue Resolver::visitCallExpr(Call* expr) {
    resolve(expr->callee.get());
    for (const auto& argument : expr->arguments) {
        resolve(argument.get());
    }
    return nullptr;
}

