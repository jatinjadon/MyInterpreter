#include "expr.h"
#include<vector>

class Parser {
private:
	std::vector<Token> tokens;
	int current = 0;

	Token peek() {
		return tokens[current];
	}
	bool isAtEnd() {
		return peek().type == _EOF;
	}
	Token previous() {
		return tokens[current - 1];
	}
	Token advance() {
		if (!isAtEnd())current++;
		return previous();
	}
	bool check(TokenType type) {
		return peek().type == type;
	}
	template <typename... Args> bool match(Args... types) {
		if ((check(types) || ...)) {
			advance();
			return true;
		}
		return false;
	}

	// CFG- Implementation

	// expression -> equality
	std::unique_ptr<Expr> expression() {
		return equality(); 
	}

	// equality -> comparison ( ( "!=" | "==" ) comparison )
	std::unique_ptr<Expr> equality() {
		std::unique_ptr<Expr> left = comparison();

		if(match(BANG_EQUAL, EQUAL_EQUAL)){
			Token op = previous();
			std::unique_ptr<Expr> right = comparison();
			left = std::make_unique<Binary>(std::move(left), op, std::move(right));
		}
		return left;
	}

	std::unique_ptr<Expr> comparison(){
		std::unique_ptr<Expr> left = term();

		if(match(LESS, LESS_EQUAL, GREATER, GREATER_EQUAL)){
			Token op = previous();
			std::unique_ptr<Expr> right = term();
			left = std::make_unique<Binary>(std::move(left), op, std::move(right));
		}
		return left;
	}

	std::unique_ptr<Expr> term() {
		std::unique_ptr<Expr> left = factor();

		if(match(MINUS, PLUS)){
			Token op = previous();
			std::unique_ptr<Expr> right = factor();
			left = std::make_unique<Binary>(std::move(left), op, std::move(right));
		}

		return left;
	}

	std::unique_ptr<Expr> factor() {
		std::unique_ptr<Expr> left = unary();
		if (match(SLASH, STAR)) {
			Token op = previous();
			std::unique_ptr<Expr> right = unary();

			left = std::make_unique<Binary>(std::move(left), op, std::move(right));
		}
		return left;
	}
	
	std::unique_ptr<Expr> unary() {
		if (match(BANG, MINUS)) {
			Token op = previous();
			std::unique_ptr<Expr> right = unary();
			return std::make_unique<Unary>(op, move(right));
		}

		return primary();
	}
	
	std::unique_ptr<Expr> primary() {
		if (match(TRUE, FALSE, NIL))return std::make_unique<Literal>(previous().lexeme);
		if (match(NUMBER, STRING))return std::make_unique<Literal>(previous().literal);

		if (match(LEFT_PAREN)) {
			std::unique_ptr<Expr> expr = expression();
			// We must find RIGHT_PAREN here, else we should report compile error
			if (match(RIGHT_PAREN)) {
				return std::make_unique<Grouping>(move(expr));
			}
			return nullptr;
		}

		return nullptr;
	}
public:
	Parser(std::vector<Token> tokens) :
		tokens(std::move(tokens)) {}

	std::unique_ptr<Expr> parse() {
		try {
			return expression();
		}
		catch (std::runtime_error& error) {
			return nullptr;
		}
	}
};