#pragma once

#include "src/std17/remove_cvref.hpp"

#define AUTO_CONSTANT(name, factoryFunction) \
constexpr std17::remove_cvref_t<decltype(factoryFunction)> name = factoryFunction
