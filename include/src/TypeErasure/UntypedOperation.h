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

#include "src/TypeErasure/CategoryEnum.h"
#include "src/TypeErasure/UntypedExpression.h"
#include "src/TypeErasure/getTypedExpressions.h"
#include "src/TypeErasure/areAllConstexpr.h"

namespace metl
{
	namespace internal
	{
		template <class UntypedExpression_t, class Input>
		class UntypedOperation
		{
			static_assert(std17::is_same_v<Input, UntypedExpression_t> || std17::is_same_v<Input, std::vector<UntypedExpression_t>>, "");

			using UntypedExpressionFunction = std::function<UntypedExpression_t(const Input&)>;
		public:

			template<class... ArgumentTypes, class TypedValueFunction>
			static UntypedOperation fromTypedValueFunction(const TypedValueFunction& typedValueFunction)
			{
				auto untypedOperationLambda = [typedValueFunction](const Input& untypedArgumentExpressions)
				{
					auto typedArgumentExpressions = getTypedExpressions<ArgumentTypes...>(untypedArgumentExpressions);
					auto typedResultExpression = makeTypedExpression(typedValueFunction, typedArgumentExpressions);

					if(areAllConstexpr(untypedArgumentExpressions))
					{
						return UntypedExpression_t::makeConstexpr(typedResultExpression());
					}
					return UntypedExpression_t::makeNonConstexpr(std::move(typedResultExpression));
				};

				return {std::move(untypedOperationLambda)};
			}

			UntypedExpression_t operator()(const Input& inputExpressions) const
			{
				return untypedExpressionFunction_(inputExpressions);
			}

		private:
			UntypedOperation(UntypedExpressionFunction untypedExpressionFunction) : untypedExpressionFunction_(std::move(untypedExpressionFunction))
			{}

			UntypedExpressionFunction untypedExpressionFunction_;
		};
	}
}
