#include "Interpreter.h"
#include<sstream>

void Interpreter::interpret(const std::vector<std::unique_ptr<Stmt>>& statements) {
	try{
		for(const auto& stmt : statements){
			execute(stmt.get());
		}
	}
	catch(const std::runtime_error& error){
		std::cerr << error.what() << std::endl;
		exit(70);
	}
}

void Interpreter::execute(Stmt* stmt){
	stmt->accept(this);
}
void Interpreter::executeBlock(const std::vector<std::unique_ptr<Stmt>> &statements, std::shared_ptr<Environment> innerEnvironment){

	std::shared_ptr<Environment> parent = this->environment;

	try{
		this->environment = innerEnvironment;

		for(const auto& statement : statements){
			execute(statement.get());
		}
		this->environment = parent;
	}
	catch(...){
		this->environment = parent;
		throw;
	}
}
LoxValue Interpreter::evaluate(Expr* expr) {
	return expr->accept(this);
}
void Interpreter::visitExpressionStmt(ExpressionStmt* stmt){
	evaluate(stmt->expression.get());
}
void Interpreter::visitPrintStmt(PrintStmt* stmt){
	LoxValue expr = evaluate(stmt->expression.get());
	std::cout << stringify(expr) << std::endl;
}
void Interpreter::visitVarStmt(VarStmt* stmt){
	LoxValue value = nullptr;
	if(stmt->initalizer.get()){
		value = evaluate(stmt->initalizer.get());
	}
	environment->define(stmt->name.lexeme, value);
}
void Interpreter::visitBlockStmt(BlockStmt* stmt){
	std::shared_ptr<Environment> newEnvironment = std::make_shared<Environment>(this->environment);
	executeBlock(stmt->statements, newEnvironment);
}
void Interpreter::visitIfStmt(IfStmt* stmt){
	LoxValue conditionValue = evaluate(stmt->condition.get());

	if(isTruthy(conditionValue)){
		execute(stmt->thenBranch.get());
	}
	else if(stmt->elseBranch.get() != nullptr){
		execute(stmt->elseBranch.get());
	}
}
void Interpreter::visitWhileStmt(WhileStmt* stmt){
	while(isTruthy(evaluate(stmt->condition.get()))){
		execute(stmt->body.get());
	}
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
LoxValue Interpreter::visitVariableExpr(Variable* expr){
	LoxValue value = environment->get(expr->name);
	return value;
}
LoxValue Interpreter::visitAssignExpr(Assign* expr){
	LoxValue value = evaluate(expr->expr.get());
	environment->assign(expr->name, value);
	return value;
}
LoxValue Interpreter::visitLogicalExpr(Logical* expr){
	LoxValue leftvalue = evaluate(expr->left.get());
	if(!isTruthy(leftvalue)){
		if(expr->op.lexeme == "and")
		return leftvalue;
	}
	else{
		if(expr->op.lexeme == "or")
		return leftvalue;
	}

	return evaluate(expr->right.get());
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