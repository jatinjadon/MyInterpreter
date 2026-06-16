#pragma once
#include<variant>
#include<string>
#include<memory>
class LoxCallable;

using LoxValue = std::variant < double, std::string, bool, std::nullptr_t, std::shared_ptr<LoxCallable>>;