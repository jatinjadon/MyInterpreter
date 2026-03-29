#pragma once
#include<variant>
#include<string>
#include<memory>

using LoxValue = std::variant < double, std::string, bool, std::nullptr_t>;