#pragma once
#include "Token.h"
#include<vector>
#include<unordered_map>

class Scanner {
private:
	std::string source;
	std::vector<Token> tokens;
	int start = 0, current = 0, line = 1;
    int& return_code;
    std::unordered_map<std::string, TokenType> keywords = {
        {"and",    AND},
        {"class",  CLASS},
        {"else",   ELSE},
        {"false",  FALSE},
        {"for",    FOR},
        {"fun",    FUN},
        {"if",     IF},
        {"nil",    NIL},
        {"or",     OR},
        {"print",  PRINT},
        {"return", RETURN},
        {"super",  SUPER},
        {"this",   THIS},
        {"true",   TRUE},
        {"var",    VAR},
        {"while",  WHILE}
    };
	bool isAtEnd();
	bool match(char expected);
	char peek();
    char peekNext()const;
	char advance();

	void addToken(TokenType type, std::string literal = "NULL");

	void scanToken();
	void stringLiteral();
	void number();
	void identifier();
    
public:
	Scanner(std::string source, int& return_code) : source(std::move(source)), return_code(return_code) {}
	std::vector<Token> scanTokens();
};