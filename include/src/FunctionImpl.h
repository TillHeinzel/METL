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

namespace metl
{
	namespace internal
	{
		template <class Expression>
		class FunctionImpl
		{
			using FunctionType = std::function<Expression(const std::vector<Expression>&)>;

		public:
			FunctionImpl(FunctionType f) : f_(f)
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
			bool resultShouldBeConstexpr(const std::vector<Expression>& v)
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

			FunctionType f_;
		};

		template <class... Ts, std::size_t... Ind, class Expression>
		auto getTypedExpressions(const std::vector<Expression>& v, std::index_sequence<Ind...>)
		{
			return std::make_tuple(v.at(Ind).template get<Ts>()...);
		}

		template <class Expression, class... Ts, class F>
		FunctionImpl<Expression> makeFunction(const F &f)
		{
			return {[f](const std::vector<Expression> &v)
			{
				auto vv = getTypedExpressions<Ts...>(v, std::make_index_sequence<sizeof...(Ts)>{});

				using retType = std::result_of_t<F(Ts...)>;

				return Expression(TypedExpression<retType>([f, vv]()
				{
					auto vals = evaluate_each(vv);
					return std17::apply(f, vals);
				};)
			}};
		}
	}
}
