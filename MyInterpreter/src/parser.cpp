#include "parser.h"

Token Parser::peek() {
    return tokens[current];
}

bool Parser::isAtEnd() {
    return peek().type == _EOF;
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    return peek().type == type;
}
template <typename... Args> 
bool Parser::match(Args... types) {
    if ((check(types) || ...)) {
        advance();
        return true;
    }
    return false;
}
Token Parser::consume(TokenType type, const std::string message) {
    if(check(type)) return advance();
    throw error(peek(), message);
}

ParseError Parser::error(Token token, const std::string& message) {
    std::string errstr;
    if(token.type == _EOF)
        errstr = "[line " + std::to_string(line) + "] Error at end: " + message;
    else
        errstr = "[line " + std::to_string(line) + "] Error at '" + token.lexeme + "': " + message;
    
    return ParseError(errstr);
}

std::unique_ptr<Expr> Parser::expression() {
    return equality(); 
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> left = comparison();
    if(!left) {
        throw error(peek(), "Expect expression.");
    }
    while(match(BANG_EQUAL, EQUAL_EQUAL)) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        if(!right) {
            throw error(peek(), "Expect expression.");
        }
        left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> left = term();
    if(!left) {
        throw error(peek(), "Expect expression.");
    }
    while(match(LESS, LESS_EQUAL, GREATER, GREATER_EQUAL)) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        if(!right) {
            throw error(peek(), "Expect expression.");
        }
        left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> left = factor();
    if(!left) {
        throw error(peek(), "Expect expression.");
    }
    while(match(MINUS, PLUS)) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        if(!right) {
            throw error(peek(), "Expect expression.");
        }
        left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> left = unary();
    if(!left) {
        throw error(peek(), "Expect expression.");
    }
    while (match(SLASH, STAR)) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        if(!right) {
            throw error(peek(), "Expect expression.");
        }
        left = std::make_unique<Binary>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::unary() {
    while (match(BANG, MINUS)) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        if(!right) {
            throw error(peek(), "Expect expression.");
        }
        return std::make_unique<Unary>(op, move(right));
    }
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match(TRUE, FALSE, NIL)) return std::make_unique<Literal>(previous().lexeme);
    if (match(NUMBER, STRING)) return std::make_unique<Literal>(previous().literal);

    if (match(LEFT_PAREN)) {
        std::unique_ptr<Expr> expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(move(expr));  
    }

    throw error(peek(), "Expect expression.");
}

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<Expr> Parser::parse() {
    try {
        return expression();
    }
    catch (std::runtime_error& error) {
        std::cout << error.what() << std::endl;
        return nullptr;
    }
}