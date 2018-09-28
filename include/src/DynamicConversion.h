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

#include <functional>
#include <type_traits>

#include "src/Utility/Typelist.h"

#include "src/DynamicExpression.h"

namespace metl
{
	namespace internal
	{
		template <class Expression>
		class DynamicConversion
		{
			using FunctionType = std::function<Expression(Expression)>;
		public:
			DynamicConversion(FunctionType f) :f_(f)
			{}

			Expression operator()(Expression v) const
			{
				auto resultExpression = f_(v);

				if(resultShouldBeConstexpr(v))
				{
					return resultExpression.evaluatedExpression();
				}

				return resultExpression;
			}

		private:
			FunctionType f_;

			bool resultShouldBeConstexpr(const Expression& v) const
			{
				return v.category() == CATEGORY::CONSTEXPR;
			}
		};

		template<class Expression, class From, class F>
		DynamicConversion<Expression> makeDynamicConversion(const F& f)
		{
			using To = std::result_of_t<F(From)>;

			static_assert(internal::isInList<From>(internal::getTypeList(Type<Expression>())), "Type casted from is not one of the types of this compiler");
			static_assert(internal::isInList<To>(internal::getTypeList(Type<Expression>())), "Type casted to is not one of the types of this compiler");

			return {[f](const Expression& from)
			{
				auto typedArgumentExpressions = std::make_tuple(from.template get<From>());
				return Expression(StaticExpression<To>{[f, typedArgumentExpressions]()
				{
					auto typedArguments = evaluate_each(typedArgumentExpressions);
					return std17::apply(f, std::move(typedArguments));
				}
				});
			}};
		}
	}
}
