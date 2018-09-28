/*
@file
Compiler.h
Declares template class Compiler, which carries all the user-defined operators, functions, etc.

Copyright 2017-2018 Till Heinzel

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

#include "src/TypeErasure/UntypedOperation.h"

namespace metl
{
	namespace internal
	{
		template<class Expression_t>
		using UntypedConversion = UntypedOperation<Expression_t, Expression_t>;

		template<class UntypedExpression_t, class ArgumentType, class F>
		UntypedConversion<UntypedExpression_t> makeDynamicConversion(const F& typedFunction)
		{
			return makeUntypedOperation<UntypedExpression_t, UntypedExpression_t, ArgumentType>(typedFunction);
		}
	}
}
