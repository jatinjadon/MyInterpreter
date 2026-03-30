#pragma once
#include "Token.h"
#include "LoxValue.h"
#include<memory>
#include<cstring>
#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>

struct Binary; struct Unary; struct Literal; struct Grouping;
struct Expr; struct Variable; struct Assign; struct Logical;

struct ExprVisitor {
	virtual LoxValue visitBinaryExpr(Binary* expr) = 0;
	virtual LoxValue visitUnaryExpr(Unary* expr) = 0;
	virtual LoxValue visitLiteralExpr(Literal* expr) = 0;
	virtual LoxValue visitGroupingExpr(Grouping* expr) = 0;
	virtual LoxValue visitVariableExpr(Variable* expr) = 0;
	virtual LoxValue visitAssignExpr(Assign* expr) = 0;
	virtual LoxValue visitLogicalExpr(Logical* expr) = 0;
	virtual ~ExprVisitor() = default;
};
struct Expr {
	virtual ~Expr() = default;
	virtual LoxValue accept(ExprVisitor* visitor) = 0;
	virtual std::string print() const = 0;
};

struct Binary : Expr {
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) :
		left(std::move(left)), op(std::move(op)), right(std::move(right)) { }

	LoxValue accept(ExprVisitor* visitor) override{
		return visitor->visitBinaryExpr(this);
	}
	std::string print() const override {
		return "(" + op.lexeme + " " + left->print() + " " + right->print() + ")";
	}
};

struct Unary :Expr{
	Token op;
	std::unique_ptr<Expr> right;

	Unary(Token op, std::unique_ptr<Expr> right) :
		op(std::move(op)), right(std::move(right)) { }

	LoxValue accept(ExprVisitor* visitor) override{
		return visitor->visitUnaryExpr(this);
	}

	std::string print() const override{
		return "(" + op.lexeme + " " + right->print() + ")";
	}
};

struct Literal : Expr {
	LoxValue value;

    Literal(std::string value) : value(move(value)) {}
    std::string print() const override{
        if(std::holds_alternative<bool>(value))return std::get<bool>(value) ? "true" : "false";
        if(std::holds_alternative<std::string>(value))return std::get<std::string>(value);
        if(std::holds_alternative<double>(value)){
            double num = std::get<double>(value);
            std::string s = std::to_string(num);

            while(s.back() == '0')s.pop_back();
            if(s.back() == '.')s += '0';
            return s;
        }
        if(std::holds_alternative<std::nullptr_t>(value))return "nil";
        return "unknown";
    }
    Literal(double value) : value(value) {}
    Literal(bool value) : value(value) {}
    Literal(std::nullptr_t value) : value(value) {}
    
    LoxValue accept(ExprVisitor* visitor) override {
        return visitor->visitLiteralExpr(this);
    }
};

struct Grouping : Expr {
	std::unique_ptr<Expr> expr;

	Grouping(std::unique_ptr<Expr> expr) :
		expr(std::move(expr)) { }
	LoxValue accept(ExprVisitor* visitor) override{
		return visitor->visitGroupingExpr(this);
	}
	std::string print() const override {
		return "(group " + expr->print() + ")";
	}
};
// node for reading a variable(print x;)
struct Variable : Expr{
	Token name;
	Variable(Token name) : name(std::move(name)) {}
	LoxValue accept(ExprVisitor* visitor) override{
		return visitor->visitVariableExpr(this);
	}
	std::string print() const{
		return name.lexeme;
	}
};
// for reassigning a variable
struct Assign : Expr{
	Token name;
	std::unique_ptr<Expr> expr;

	Assign(Token name, std::unique_ptr<Expr> expr) : 
		name(std::move(name)), expr(std::move(expr)) {}
	LoxValue accept(ExprVisitor* visitor) override{
		return visitor->visitAssignExpr(this);
	}
	std::string print() const{
		return "(= " + name.lexeme + " " + expr->print() + ")";
	}
};
struct Logical : Expr{
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	Logical(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) : 
		left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
	LoxValue accept(ExprVisitor* visitor) override{
		return visitor->visitLogicalExpr(this);
	}
	std::string print() const override{
		return "(" + op.lexeme + " " + left->print() + " " + right->print() + ")";
	}
};
