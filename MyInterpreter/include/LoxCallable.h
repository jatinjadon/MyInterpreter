#include "LoxValue.h"
#include<vector>

class Interpreter;

class LoxCallable {
public:
    virtual ~LoxCallable() = default;
    virtual int arity() = 0;
    virtual LoxValue call(Interpreter* Interpreter, const std::vector<LoxValue> &arguments) = 0;
    virtual std::string toString() = 0;
};