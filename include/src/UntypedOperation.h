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

#include "src/UntypedExpression.h"

namespace metl
{
	namespace internal
	{
		template <class Expression, class Input>
		class UntypedOperation
		{
		};

		template<class UntypedExpression_t, class StaticFunction, class StaticArgumentExpressions>
		UntypedExpression_t makeUntypedExpression(StaticFunction staticFunction, StaticArgumentExpressions staticArgumentExpressions)
		{
			auto staticExpressionLambda = [
				staticFunction = std::move(staticFunction),
				staticArgumentExpressions = std::move(staticArgumentExpressions)]
				()
			{
				auto staticArguments = evaluate_each(staticArgumentExpressions);
				return std17::apply(staticFunction, std::move(staticArguments));
			};

			using RetType = decltype(staticExpressionLambda());

			return UntypedExpression_t(TypedExpression<RetType>(staticExpressionLambda));
		}
	}
}
