#pragma once

#include "src/std17/remove_cvref.hpp"

#define AUTO_MEMBER(membername, factoryFunction) \
std17::remove_cvref_t<decltype(factoryFunction)> membername = factoryFunction
