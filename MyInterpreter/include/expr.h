#pragma once
#include "Token.h"
#include<memory>
#include<cstring>
#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>

struct Binary; struct Unary; struct Literal; struct Grouping;

struct Expr {
	virtual ~Expr() = default;
	virtual std::string print() = 0;
};

struct Binary : Expr {
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) :
		left(std::move(left)), op(std::move(op)), right(std::move(right)) { }

	std::string print() override {
		return "(" + op.lexeme + " " + left->print() + " " + right->print() + ")";
	}
};

struct Unary :Expr{
	Token op;
	std::unique_ptr<Expr> right;

	Unary(Token op, std::unique_ptr<Expr> right) :
		op(std::move(op)), right(std::move(right)) { }

	std::string print() override{
		return "(" + op.lexeme + " " + right->print() + ")";
	}
};

struct Literal : Expr {
	std::string literal;

	Literal(std::string literal) :
		literal(std::move(literal)) { }
	std::string print() override{
		return literal;
	}
};

struct Grouping : Expr {
	std::unique_ptr<Expr> expr;

	Grouping(std::unique_ptr<Expr> expr) :
		expr(std::move(expr)) { }
	std::string print() override {
		return "(group " + expr->print() + ")";
	}
};
