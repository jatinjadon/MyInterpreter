#include "Scanner.h"
#include<iostream>

bool Scanner::isAtEnd() {
	return current >= (int)source.size();
}

bool Scanner::match(char expected) {
	if (isAtEnd() || source[current] != expected)return false;
	current++;

	return true;
}

char Scanner::peek() {
	if (isAtEnd())return '\0';
	return source[current];
}
char Scanner::peekNext() const {
    if (current + 1 >= (int)source.length()) return '\0';
    return source[current + 1];
}
char Scanner::advance() {
	return source[current++];
}

void Scanner::addToken(TokenType type, std::string literal) {
    std::string lexeme = source.substr(start, current - start);
    tokens.push_back({ type, lexeme, literal, line });
}

void Scanner::stringLiteral() {
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n')line++;
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "[line " << line << "] Error: Unterminated string." << std::endl;
        return_code = 65;
        return;
    }
    advance();

    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}
void Scanner::number() {
    while (isdigit(peek()))advance();

    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek()))advance();
    }

    std::string value = source.substr(start, current - start);
    if (value.find('.') == std::string::npos)
        value += ".0";
    else {
        while (value.back() == '0')value.pop_back();
        if (value.back() == '.')value += '0';
    }

    addToken(TokenType::NUMBER, value);
}
void Scanner::identifier() {
    while (isalnum(peek()) || peek() == '_')
        advance();

    std::string text = source.substr(start, current - start);
    TokenType type;

    if (keywords.find(text) != keywords.end())
        type = keywords[text];
    else
        type = TokenType::IDENTIFIER;

    addToken(type);
}
void Scanner::scanToken() {
	char c = advance();
	switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;

        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            }
            else {
                addToken(SLASH);
            }
            break;

        case ' ':
        case '\r':
        case '\t':
            break; 
        case '\n':
            line++;
            break;

        case '"':
            stringLiteral(); break;

        default:
            if (isdigit(c))
                number();
            else if (isalpha(c) || c == '_')
                identifier();
            else{
                std::cerr << "Unexpected character at line " << line << ": " << c << std::endl;
                return_code = 65;
            }
            break;
	}
}
std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({ _EOF, "", "null", line });
    return tokens;
}
