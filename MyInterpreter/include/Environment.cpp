#include "Environment.h"

void Environment::define(const std::string& name, const LoxValue& value){
    variables[name] = value;
}
LoxValue Environment::get(const Token& name) const{
    auto it = variables.find(name.lexeme);
    if(it != variables.end()){
        return it->second;
    }
    if(enclosing != nullptr){
        enclosing->get(name);
    }
    throw std::runtime_error("Undefined variable: '" + name.lexeme + "'.");
}
void Environment::assign(const Token &name, LoxValue& value){
    auto it = variables.find(name.lexeme);
    if(it != variables.end()){
        variables[name.lexeme] = value;
        return;
    }
    if(enclosing != nullptr){
        enclosing->assign(name, value);
    }
    throw std::runtime_error("Undefined variable: '" + name.lexeme + "'.");   
}