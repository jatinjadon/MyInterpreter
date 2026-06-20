#pragma once
#include "expr.h"
#include "stmt.h"
#include "Environment.h"
#include "ReturnException.h"
#include<stdexcept>
#include<unordered_map>

class Interpreter : public ExprVisitor, public StmtVisitor {
public:
	bool isREPLmode = false;
	void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);
	LoxValue evaluate(Expr* expr);
	Interpreter();
	void executeBlock(const std::vector<std::unique_ptr<Stmt>> &statements,
                    std::shared_ptr<Environment> innerEnvironment);
	std::string stringify(const LoxValue& value);
	void resolve(Expr* expr, int depth); // store the distance between current scope and the scope where the variable is declared in environment for later use
private:
	std::shared_ptr<Environment> globals = std::make_shared<Environment>();
	std::shared_ptr<Environment> environment = globals;
	std::unordered_map<Expr*, int> locals; // store the distance between current scope and the scope where the variable is declared for each variable expression

	void execute(Stmt* stmt);

	LoxValue visitBinaryExpr(Binary* expr) override;
	LoxValue visitUnaryExpr(Unary* expr) override;
	LoxValue visitLiteralExpr(Literal* expr) override;
	LoxValue visitGroupingExpr(Grouping* expr) override;
	LoxValue visitVariableExpr(Variable* expr) override;
	LoxValue visitAssignExpr(Assign* expr) override;
	LoxValue visitLogicalExpr(Logical* expr) override;
	LoxValue visitCallExpr(Call* expr) override;

	void visitPrintStmt(PrintStmt* stmt) override;
	void visitExpressionStmt(ExpressionStmt* stmt) override;
	void visitVarStmt(VarStmt* stmt) override;
	void visitBlockStmt(BlockStmt* stmt) override;
	void visitIfStmt(IfStmt* stmt) override;
	void visitWhileStmt(WhileStmt* stmt) override;
	void visitFunctionStmt(FunctionStmt* stmt) override;
	void visitReturnStmt(ReturnStmt* stmt) override;

	bool isTruthy(const LoxValue &value);
	bool isEqual(const LoxValue& a, const LoxValue& b);
	void checkNumberOperand(const LoxValue &operand);
	void checkNumberOperands(const LoxValue &left, const LoxValue &right);
	LoxValue getVariable(const Token& name, Expr* expr);
};