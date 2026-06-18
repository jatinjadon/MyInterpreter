#include "Environment.h"
#include "Interpreter.h"
#include "LoxCallable.h"
#include "stmt.h"
#include<memory>

class LoxFunction : public LoxCallable {
private:
  FunctionStmt *declaration;
  std::shared_ptr<Environment> closure;

public:
  LoxFunction(FunctionStmt *declaration, std::shared_ptr<Environment> closure)
      : declaration(declaration), closure(closure) {}

  int arity() override { return declaration->params.size(); }

  LoxValue call(Interpreter *interpreter, const std::vector<LoxValue> &arguments) override {

    auto environment = std::make_shared<Environment>(closure);

    for (size_t i = 0; i < declaration->params.size(); i++) {
      environment->define(declaration->params[i].lexeme, arguments[i]);
    }

    try {
        interpreter->executeBlock(declaration->body, environment);
    }
	catch (const ReturnException& returnValue) {
		return returnValue.value;
	}
    return nullptr;
  }

  std::string toString() { return "<fn " + declaration->name.lexeme + ">"; }
};