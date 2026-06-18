#include "Parser.h"
#include "stmt.h"

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
        errstr = "[line " + std::to_string(token.line) + "] Error at end: " + message;
    else
        errstr = "[line " + std::to_string(token.line) + "] Error at '" + token.lexeme + "': " + message;
    
	hadError = true;
    std::cerr << errstr << std::endl;
    return ParseError(errstr);
}

// for error recovery
void Parser::synchronize() {
    while (!isAtEnd()) {
        if (previous().type == SEMICOLON) return;

        switch (peek().type) {
        case CLASS:
        case FUN:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case PRINT:
        case RETURN:
        case LEFT_BRACE:
            return;
        default:
            break;
        }

        advance();
    }
}
std::unique_ptr<Expr> Parser::expression() {
    return assignment(); 
}
std::unique_ptr<Expr> Parser::assignment(){
    std::unique_ptr<Expr> left = logicalOr();

    if(match(EQUAL)){
        Token equals = previous();

        // need to call assignment (eg. a = b = 5)
        std::unique_ptr<Expr> expr = assignment();

        // left side should be a Variable, else it would be invalid assignment(eg. 5+2 = 10;)
        if(Variable* v = dynamic_cast<Variable*>(left.get())){
            Token name = v->name;
            return std::make_unique<Assign>(std::move(name), std::move(expr));
        }
        throw error(equals, "User Tried to do Invalid assignment.");
    }
    return left;
}
std::unique_ptr<Expr> Parser::logicalOr(){
    std::unique_ptr<Expr> left = logicalAnd();

    // while -> for left associativity (eg. 1 or 2 or 4)
    while(match(OR)){
        Token op = previous();
        std::unique_ptr<Expr> right = logicalAnd();

        left = std::make_unique<Logical>(std::move(left), std::move(op), std::move(right));
    }
    return left;
}
std::unique_ptr<Expr> Parser::logicalAnd(){
    std::unique_ptr<Expr> left = equality();
    while(match(AND)){
        Token op = previous();
        std::unique_ptr<Expr> right = equality();

        left = std::make_unique<Logical>(std::move(left), std::move(op), std::move(right));
    }
    return left;
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
    return call();
}
std::unique_ptr<Expr> Parser::call() {
	std::unique_ptr<Expr> expr = primary();
	while (true) {
		if (match(LEFT_PAREN)) {
			expr = finishCall(move(expr));
		}
		else break;
	}
	return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if(match(FALSE)){
        return std::make_unique<Literal>(false);
    }
    if(match(TRUE)){
        return std::make_unique<Literal>(true);
    }
    if(match(NIL)){
        return std::make_unique<Literal>(nullptr);
    }
    if(match(NUMBER)){
        double num = std::stod(previous().lexeme);
        return std::make_unique<Literal>(num);
    }
    if(match(IDENTIFIER)){
        return std::make_unique<Variable>(previous());
    }
    if (match(STRING)) return std::make_unique<Literal>(previous().literal);

    if (match(LEFT_PAREN)) {
        std::unique_ptr<Expr> expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(move(expr));  
    }

    throw error(peek(), "Expect expression.");
}
std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
	std::vector<std::unique_ptr<Expr>> arguments;
	if (!check(RIGHT_PAREN)) {
		do {
			if (arguments.size() >= 255) {
				throw error(peek(), "Can't have more than 255 arguments.");
			}
			arguments.push_back(expression());
		} while (match(COMMA));
	}
	Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");
	return std::make_unique<Call>(std::move(callee), std::move(paren), std::move(arguments));
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
      // Look for the 'fun' keyword first!
      if (match(FUN))
        return function("function");

      if (match(VAR))
        return varStatement();
      return statement();
    } catch (ParseError error) {
      synchronize();
      return nullptr;
    }
}
std::unique_ptr<Stmt> Parser::function(std::string kind){
    Token name = consume(IDENTIFIER, "Expect " + kind + " name.");

    consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check(RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                throw error(peek(), "Can't have more than 255 parameters.");
            }

            parameters.push_back(consume(IDENTIFIER, "Expect parameter name."));
        } while (match(COMMA));
    }
    consume(RIGHT_PAREN, "Expect ')' after parameters.");

    consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::unique_ptr<Stmt>> body = block();
    return std::make_unique<FunctionStmt>(std::move(name), std::move(parameters), std::move(body));
}

std::unique_ptr<Stmt> Parser::varStatement(){
    Token name = consume(IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Expr> initializer = nullptr;

    if(match(EQUAL)){
        initializer = expression();
    }
    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<VarStmt>(std::move(name), std::move(initializer));
}
std::unique_ptr<Stmt> Parser::statement(){
    if(match(IF)){
        return ifStatement();
    }
    if(match(PRINT)){
        return printStatement();
    }

    if(match(LEFT_BRACE)){
        return std::make_unique<BlockStmt>(block());
    }
    if(match(WHILE)){
        return whileStatement();
    }
    if(match(FOR)){
        return forStatement();
    }
	if (match(RETURN)) {
		return returnStatement();
	}
    return expressionStatement();
}
std::unique_ptr<Stmt> Parser::whileStatement(){
    consume(LEFT_PAREN, "Expect '(' after while keyword.");
    std::unique_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after while's condition.");

    std::unique_ptr<Stmt> body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}
std::unique_ptr<Stmt> Parser::forStatement(){
    /*
    for loop is treated as like this
    {
        initializer;
        while(condition){
            ...
        }
    }
    */
    consume(LEFT_PAREN, "Expect '(' after for keyword.");
    std::unique_ptr<Stmt> initializer = nullptr;
    
    if(!match(SEMICOLON)){
        if(match(VAR))
        initializer = varStatement();

        else
        initializer = expressionStatement();
    }

    std::unique_ptr<Expr> condition;
    if(match(SEMICOLON)){
        condition = nullptr;
    }
    else{
        condition = expression();
        consume(SEMICOLON, "Expect ';' after loop condition.");
    }

    std::unique_ptr<Expr> increment;
    if(match(RIGHT_PAREN))
    increment = nullptr;

    else{
        increment = expression();
        consume(RIGHT_PAREN, "Expect ')' after for clauses.");
    }

    std::unique_ptr<Stmt> body = statement();

    if(increment != nullptr){
        std::vector<std::unique_ptr<Stmt>> statements;
        statements.push_back(std::move(body));
        statements.push_back(std::make_unique<ExpressionStmt>(std::move(increment)));
        body = std::make_unique<BlockStmt>(std::move(statements));
    }

    if(condition == nullptr)
    condition = std::make_unique<Literal>(true);

    body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));

    if(initializer != nullptr){
        std::vector<std::unique_ptr<Stmt>> statements;
        statements.push_back(std::move(initializer));
        statements.push_back(std::move(body));
        body = std::make_unique<BlockStmt>(std::move(statements));
    }

    return body;
}
std::unique_ptr<Stmt> Parser::ifStatement(){
    consume(LEFT_PAREN, "Expect '(' after if.");
    std::unique_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after if's condition.");

    std::unique_ptr<Stmt> thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;

    if(match(ELSE)){
        elseBranch = statement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}
std::vector<std::unique_ptr<Stmt>> Parser::block(){
    std::vector<std::unique_ptr<Stmt>> statements;
    while(!check(RIGHT_BRACE) && !isAtEnd()){
        statements.push_back(declaration());
    }
    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}
std::unique_ptr<Stmt> Parser::printStatement(){
    std::unique_ptr<Expr> expr = expression();
    consume(SEMICOLON, "Expect ; after expression.");
    return std::make_unique<PrintStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::expressionStatement(){
    std::unique_ptr<Expr> expr = expression();
    consume(SEMICOLON, "Expect ; after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::vector<std::unique_ptr<Stmt>> Parser::run(){
    std::vector<std::unique_ptr<Stmt>> statements;

    while(!isAtEnd()){
        statements.push_back(declaration());    
    }
    return statements;
}
std::unique_ptr<Stmt> Parser::returnStatement() {
	Token keyword = previous();
	std::unique_ptr<Expr> value = nullptr;

	if (!check(SEMICOLON)) {
		value = expression();
	}
	consume(SEMICOLON, "Expect ';' after return value.");
	return std::make_unique<ReturnStmt>(std::move(keyword), std::move(value));
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