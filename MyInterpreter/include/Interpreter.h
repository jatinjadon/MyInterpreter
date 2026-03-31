#pragma once
#include "expr.h"
#include "stmt.h"
#include "Environment.h"
#include<stdexcept>

class Interpreter : public ExprVisitor, public StmtVisitor {
public:
	void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);
	LoxValue evaluate(Expr* expr);
private:
	std::shared_ptr<Environment> environment = std::make_shared<Environment>();

	void executeBlock(const std::vector<std::unique_ptr<Stmt>> &statements,
                    std::shared_ptr<Environment> innerEnvironment);
	void execute(Stmt* stmt);

	LoxValue visitBinaryExpr(Binary* expr) override;
	LoxValue visitUnaryExpr(Unary* expr) override;
	LoxValue visitLiteralExpr(Literal* expr) override;
	LoxValue visitGroupingExpr(Grouping* expr) override;
	LoxValue visitVariableExpr(Variable* expr) override;
	LoxValue visitAssignExpr(Assign* expr) override;
	LoxValue visitLogicalExpr(Logical* expr) override;

	void visitPrintStmt(PrintStmt* stmt) override;
	void visitExpressionStmt(ExpressionStmt* stmt) override;
	void visitVarStmt(VarStmt* stmt) override;
	void visitBlockStmt(BlockStmt* stmt) override;
	void visitIfStmt(IfStmt* stmt) override;
	void visitWhileStmt(WhileStmt* stmt) override;
	
	bool isTruthy(const LoxValue &value);
	bool isEqual(const LoxValue& a, const LoxValue& b);
	std::string stringify(const LoxValue &value);
	void checkNumberOperand(const LoxValue &operand);
	void checkNumberOperands(const LoxValue &left, const LoxValue &right);
};