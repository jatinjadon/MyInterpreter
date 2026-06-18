#pragma once
#include "LoxValue.h"

struct ReturnException {
  LoxValue value;

  explicit ReturnException(LoxValue value) : value(std::move(value)) {}
};
