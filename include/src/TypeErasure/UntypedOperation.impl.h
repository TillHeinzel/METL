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

#include "src/TypeErasure/UntypedOperation.h"

#include "src/TypeErasure/UntypedExpression.h"
#include "src/TypeErasure/getTypedExpressions.h"
#include "src/TypeErasure/areAllConstexpr.h"


namespace metl
{
	namespace internal
	{
		template <class UntypedExpression_t, class Input>
		template <class ... ArgumentTypes, class TypedValueFunction>
		UntypedOperation<UntypedExpression_t, Input> UntypedOperation<UntypedExpression_t, Input>::fromTypedValueFunction(const TypedValueFunction& typedValueFunction)
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

		template <class UntypedExpression_t, class Input>
		UntypedExpression_t	UntypedOperation<UntypedExpression_t, Input>::operator() (const Input &inputExpressions) const
		{
			return untypedExpressionFunction_(inputExpressions);
		}

		template <class UntypedExpression_t, class Input> 
		UntypedOperation<UntypedExpression_t, Input>::UntypedOperation(UntypedExpressionFunction untypedExpressionFunction)
			: untypedExpressionFunction_(std::move(untypedExpressionFunction))
		{}


	}
}
