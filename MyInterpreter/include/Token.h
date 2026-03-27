#pragma once
#include<iostream>
#include<string>
#include<variant>

enum TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    _EOF
};

struct Token {
    TokenType type;
    std::string lexeme; // the exact text read from the source
    std::string literal;
    int line = 0;

    Token(TokenType type, std::string lexeme, std::string literal, int line):
        type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) { }

    std::string toString() const {
        std::string typeStr;

        switch (type) {
        case LEFT_PAREN: typeStr = "LEFT_PAREN"; break;
        case RIGHT_PAREN: typeStr = "RIGHT_PAREN"; break;
        case LEFT_BRACE: typeStr = "LEFT_BRACE"; break;
        case RIGHT_BRACE: typeStr = "RIGHT_BRACE"; break;
        case COMMA: typeStr = "COMMA"; break;
        case DOT: typeStr = "DOT"; break;
        case MINUS: typeStr = "MINUS"; break;
        case PLUS: typeStr = "PLUS"; break;
        case SEMICOLON: typeStr = "SEMICOLON"; break;
        case SLASH: typeStr = "SLASH"; break;
        case STAR: typeStr = "STAR"; break;
        case BANG: typeStr = "BANG"; break;
        case BANG_EQUAL: typeStr = "BANG_EQUAL"; break;
        case EQUAL: typeStr = "EQUAL"; break;
        case EQUAL_EQUAL: typeStr = "EQUAL_EQUAL"; break;
        case GREATER: typeStr = "GREATER"; break;
        case GREATER_EQUAL: typeStr = "GREATER_EQUAL"; break;
        case LESS: typeStr = "LESS"; break;
        case LESS_EQUAL: typeStr = "LESS_EQUAL"; break;
        case IDENTIFIER: typeStr = "IDENTIFIER"; break;
        case STRING: typeStr = "STRING"; break;
        case NUMBER: typeStr = "NUMBER"; break;
        case AND: typeStr = "AND"; break;
        case CLASS: typeStr = "CLASS"; break;
        case ELSE: typeStr = "ELSE"; break;
        case FALSE: typeStr = "FALSE"; break;
        case FUN: typeStr = "FUN"; break;
        case FOR: typeStr = "FOR"; break;
        case IF: typeStr = "IF"; break;
        case NIL: typeStr = "NIL"; break;
        case OR: typeStr = "OR"; break;
        case PRINT: typeStr = "PRINT"; break;
        case RETURN: typeStr = "RETURN"; break;
        case SUPER: typeStr = "SUPER"; break;
        case THIS: typeStr = "THIS"; break;
        case TRUE: typeStr = "TRUE"; break;
        case VAR: typeStr = "VAR"; break;
        case WHILE: typeStr = "WHILE"; break;
        case _EOF: typeStr = "EOF"; break;
        }
        return typeStr + " " + lexeme + " " + literal;
    }
}; 