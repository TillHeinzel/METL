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
#include "src/std17/is_same_v.h"

#include "Associativity.h"
#include "EvaluateConstexpr.h"

#include "CategoryEnum.h"

#include "src/UntypedExpression.h"
#include "src/getTypedExpressions.h"

namespace metl
{
	namespace internal
	{
		template <class UntypedExpression_t, class Input>
		class UntypedOperation
		{
			static_assert(std17::is_same_v<Input, UntypedExpression_t> || std17::is_same_v<Input, std::vector<UntypedExpression_t>>, "");

			using FunctionType = std::function<UntypedExpression_t(const Input&)>;
		public:
			UntypedOperation(FunctionType f) : f_(f)
			{}

			UntypedExpression_t operator()(const Input& v) const
			{
				auto resultExpression = f_(v);

				if(resultShouldBeConstexpr(v))
				{
					return resultExpression.evaluatedExpression();
				}

				return resultExpression;
			}

		protected:
			FunctionType f_;

			bool resultShouldBeConstexpr(const UntypedExpression_t& v) const
			{
				return v.category() == CATEGORY::CONSTEXPR;
			}

			bool resultShouldBeConstexpr(const std::vector<UntypedExpression_t>& v) const
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

		template<class UntypedExpression_t, class Input, class... ArgumentTypes, class F>
		UntypedOperation<UntypedExpression_t, Input> makeUntypedOperation(const F& typedFunction)
		{
			auto untypedOperationLambda = [typedFunction](const Input& untypedArgumentExpressions)
			{
				auto typedArgumentExpressions = getTypedExpressions<ArgumentTypes...>(untypedArgumentExpressions);
				return UntypedExpression_t(makeTypedExpression(typedFunction, typedArgumentExpressions));
			};

			return {std::move(untypedOperationLambda)};
		}
	}
}
