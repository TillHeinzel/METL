/*
@file
Compiler.impl.h
Implements functions for class Compiler

Copyright 2017 Till Heinzel

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once
#include "Compiler.h"

#include <tao/pegtl.hpp>

#include "grammar.h"
#include "actions.h"

#include "nameMangling.h"
#include "Stack.impl.h"

#include "CompilerHelpers.h"
#include "CompilerBits.impl.h"
#include <locale>

namespace metl
{
	namespace internal
	{
		template <class LiteralConverters, class ... Ts>
		Compiler<LiteralConverters, Ts...>::Compiler(const LiteralConverters& literalConverters) :
			stack_(bits_),
			literalConverters_(literalConverters)
		{}

		template <class LiteralConverters, class... Ts>
		template <class T>
		constexpr TYPE Compiler<LiteralConverters, Ts...>::type()
		{
			return classToType2<T, Ts...>();
		}
	}
}