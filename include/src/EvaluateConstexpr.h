/*
@file
Stack.impl.h
implementation of functions for template Stack, declared in Stack.fwd.h

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
#include "src/VarExpression.h"

namespace metl
{
	namespace internal
	{

		template<class T>
		struct constexprFunction
		{
			T v;

			T operator()() const { return v; }
		};

		template<class Expression, class T>
		Expression makeConstExpression(const T& v)
		{
			auto f = constexprFunction<T>{ v };

			return Expression(exprType<T>(f), CATEGORY::CONSTEXPR);
		}
	}
}
