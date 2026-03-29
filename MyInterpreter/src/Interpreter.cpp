#include "Interpreter.h"
#include<sstream>

void Interpreter::interpret(Expr* expression) {
	try {
		LoxValue value = evaluate(expression);
		std::cout << stringify(value) << std::endl;
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Runtime Error: " << error.what() << std::endl;
		exit(70);
	}
}

LoxValue Interpreter::evaluate(Expr* expr) {
	return expr->accept(this);
}

LoxValue Interpreter::visitBinaryExpr(Binary* expr) {
	LoxValue left = evaluate(expr->left.get());
	Token op = expr->op;
	LoxValue right = evaluate(expr->right.get());

	switch (expr->op.type) {
	case PLUS:
		if (std::holds_alternative<double>(left) &&
			std::holds_alternative<double>(right))
			return std::get<double>(left) + std::get<double>(right);
		if (std::holds_alternative<std::string>(left) &&
			std::holds_alternative<std::string>(right))
			return std::get<std::string>(left) + std::get<std::string>(right);
		throw std::runtime_error("Operands must be two numbers or two strings.");
	case MINUS:
		checkNumberOperands(left, right);
		return std::get<double>(left) - std::get<double>(right);
	case STAR:
		checkNumberOperands(left, right);
		return std::get<double>(left) * std::get<double>(right);
	case SLASH:
		checkNumberOperands(left, right);
		return std::get<double>(left) / std::get<double>(right);
	case EQUAL_EQUAL:
		return isEqual(left, right);
	case BANG_EQUAL:
		return !isEqual(left, right);
	case LESS:
		checkNumberOperands(left, right);
		return std::get<double>(left) < std::get<double>(right);
	case LESS_EQUAL:
		checkNumberOperands(left, right);
		return std::get<double>(left) <= std::get<double>(right);
	case GREATER:
		checkNumberOperands(left, right);
		return std::get<double>(left) > std::get<double>(right);
	case GREATER_EQUAL:
		checkNumberOperands(left, right);
		return std::get<double>(left) >= std::get<double>(right);
	default:
		return nullptr;
	}
}

LoxValue Interpreter::visitUnaryExpr(Unary* expr) {
	Token op = expr->op;
	LoxValue right = evaluate(expr->right.get());

	switch (op.type) {
		case BANG:
			return !isTruthy(right);
		case MINUS:
			checkNumberOperand(right);
			return -std::get<double>(right);
		default:
			return nullptr;
	}
}

LoxValue Interpreter::visitGroupingExpr(Grouping* expr) {
	return evaluate(expr->expr.get());
}

LoxValue Interpreter::visitLiteralExpr(Literal* expr) {
	return expr->value;
}
bool Interpreter::isTruthy(const LoxValue& value) {
	if (std::holds_alternative<std::nullptr_t>(value))return false;
	if (std::holds_alternative<bool>(value))return std::get<bool>(value);

	return true;
}

bool Interpreter::isEqual(const LoxValue& a, const LoxValue& b) {
	return a == b;
}

void Interpreter::checkNumberOperand(const LoxValue& operand) {
	if (std::holds_alternative<double>(operand))return;
	throw std::runtime_error("Operand must be a number.");
}

void Interpreter::checkNumberOperands(const LoxValue& left, const LoxValue& right) {
	if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))return;
	throw std::runtime_error("Operands must be numbers.");
}

std::string Interpreter::stringify(const LoxValue& value) {
	if (std::holds_alternative<std::nullptr_t>(value))return "nil";
	if (std::holds_alternative<bool>(value))return std::get<bool>(value) ? "true" : "false";
	if (std::holds_alternative<std::string>(value))return std::get<std::string>(value);

	if (std::holds_alternative<double>(value)) {
		std::ostringstream oss;
        oss << std::get<double>(value);
        return oss.str();
	}

	return "Unknown";
}