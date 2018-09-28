/*
@file
Compiler.h
Declares template class Compiler, which carries all the user-defined operators,
functions, etc.

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
#include <vector>

#include "src/Utility/evaluate_each.h"
#include "src/std17/apply.h"

#include "Associativity.h"
#include "EvaluateConstexpr.h"

#include "CategoryEnum.h"

#include "src/DynamicExpression.h"

namespace metl
{
	namespace internal
	{
		template <class Expression>
		class DynamicFunction
		{
			using FunctionType = std::function<Expression(const std::vector<Expression>&)>;
		public:
			DynamicFunction(FunctionType f) : f_(f)
			{}

			Expression operator()(const std::vector<Expression>& v) const
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

			bool resultShouldBeConstexpr(const std::vector<Expression>& v) const
			{
				bool shouldBeConst = true;
				for(const auto &expr : v)
				{
					if(expr.category() == CATEGORY::DYNEXPR)
					{
						shouldBeConst = false;
						break;
					}
				}
				return shouldBeConst;
			}

		};



		template <class... Ts, std::size_t... Ind, class Expression>
		auto getTypedExpressions_impl(const std::vector<Expression>& v, std::index_sequence<Ind...>)
		{
			return std::make_tuple(v.at(Ind).template get<Ts>()...);
		}

		template <class... Ts, class Expression>
		auto getTypedExpressions(const std::vector<Expression>& v)
		{
			return getTypedExpressions_impl<Ts...>(v, std::make_index_sequence<sizeof...(Ts)>{});
		}


		template <class Expression, class... ArgumentTypes, class TypedFunction>
		DynamicFunction<Expression> makeDynamicFunction(const TypedFunction& typedFunction)
		{
			return {[typedFunction](const std::vector<Expression>& v)
			{
				auto typedArgumentExpressions = getTypedExpressions<ArgumentTypes...>(v);

				using retType = std::result_of_t<TypedFunction(ArgumentTypes...)>;

				return Expression(StaticExpression<retType>([typedFunction, typedArgumentExpressions]()
				{
					auto typedArguments = evaluate_each(typedArgumentExpressions);
					return std17::apply(typedFunction, typedArguments);
				}));
			}};
		}
	}
}
