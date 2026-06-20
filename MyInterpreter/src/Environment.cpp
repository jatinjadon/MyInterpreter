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
        return enclosing->get(name);
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
        return;
    }
    
    throw std::runtime_error("Undefined variable: '" + name.lexeme + "'.");   
}
LoxValue Environment::getAt(int distance, const std::string &name){
    Environment* env = ancestor(distance);
    if(env == nullptr){
        throw std::runtime_error("Undefined variable: '" + name + "'.");
    }
    auto it = env->variables.find(name);
    if(it != env->variables.end()){
        return it->second;
    }
    throw std::runtime_error("Undefined variable: '" + name + "'.");
}
void Environment::assignAt(int distance, const Token &name, LoxValue &value){
    Environment* env = ancestor(distance);
    if(env == nullptr){
        throw std::runtime_error("Undefined variable: '" + name.lexeme + "'.");
    }
    env->variables[name.lexeme] = value;
}
Environment* Environment::ancestor(int distance){
    Environment* env = this;
    for(int i = 0; i < distance; ++i){
        if(env->enclosing == nullptr){
            return nullptr;
        }
        env = env->enclosing.get();
    }
    return env;
}