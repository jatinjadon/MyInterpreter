#pragma once
#include "expr.h"
#include "Token.h"
#include "stmt.h"
#include<vector>

class ParseError : public std::runtime_error{
public:
	ParseError(const std::string error) : std::runtime_error(error){}
};

class Parser{
private:
	std::vector<Token> tokens; 
	int current = 0;
	int line = 1;

    Token peek();
    Token previous();
    bool check(TokenType type);
    bool isAtEnd();
    Token advance();
    template <typename... Args> bool match(Args... types) ;
    Token consume(TokenType type, const std::string message);
    ParseError error(Token token, const std::string& message);
    void synchronize();

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> logicalOr();
    std::unique_ptr<Expr> logicalAnd();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::vector<std::unique_ptr<Stmt>> block();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<Stmt> varStatement();
    std::unique_ptr<Stmt> ifStatement();

public: 
    Parser(std::vector<Token> tokens);
    std::vector<std::unique_ptr<Stmt>> run();
    std::unique_ptr<Expr> parse();
};