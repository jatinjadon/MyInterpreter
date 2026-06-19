#pragma once
#include "LoxValue.h"
#include "Token.h"
#include<stdexcept>
#include<string>
#include<unordered_map>

class Environment{
private:
    std::unordered_map<std::string, LoxValue> variables;
    std::shared_ptr<Environment > enclosing;
    Environment* ancestor(int distance);

public:
    Environment() : enclosing(nullptr){};
    Environment(std::shared_ptr<Environment> enclosing) : 
        enclosing(enclosing) {}

    void define(const std::string &name, const LoxValue &value);
    LoxValue get(const Token &name)const;
    void assign(const Token &name, LoxValue &value);
    void assignAt(int distance, const Token &name, LoxValue &value);
    LoxValue getAt(int distance, const std::string &name) ;
};