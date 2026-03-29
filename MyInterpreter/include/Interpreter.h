#pragma once
#include "expr.h"
#include<stdexcept>

class Interpreter : public ExprVisitor {
public:
	void interpret(Expr* expression);
private:
	LoxValue evaluate(Expr* expr);

	LoxValue visitBinaryExpr(Binary* expr) override;
	LoxValue visitUnaryExpr(Unary* expr) override;
	LoxValue visitLiteralExpr(Literal* expr) override;
	LoxValue visitGroupingExpr(Grouping* expr) override;

	bool isTruthy(const LoxValue &value);
	bool isEqual(const LoxValue& a, const LoxValue& b);
	std::string stringify(const LoxValue &value);
	void checkNumberOperand(const LoxValue &operand);
	void checkNumberOperands(const LoxValue &left, const LoxValue &right);
};