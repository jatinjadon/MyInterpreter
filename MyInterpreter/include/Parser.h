#pragma once
#include "expr.h"
#include "Token.h"
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
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

public: 
    Parser(std::vector<Token> tokens);
    std::unique_ptr<Expr> parse();
};