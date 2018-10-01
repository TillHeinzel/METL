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

#include "src/TypeErasure/UntypedOperation.fwd.h"

#include <functional>
#include <vector>

#include "src/std17/is_same_v.h"
#include "src/Utility/is_specialization_of.h"

#include "src/TypeErasure/UntypedExpression.h"

namespace metl
{
	namespace internal
	{
		template <class UntypedExpression_t, class Input>
		class UntypedOperation
		{
			static_assert(is_specialization_of_v<UntypedExpression_t, UntypedExpression>, "");
			static_assert(std17::is_same_v<Input, UntypedExpression_t> || std17::is_same_v<Input, std::vector<UntypedExpression_t>>, "");

			using UntypedExpressionFunction = std::function<UntypedExpression_t(const Input&)>;
		public:
			template<class... ArgumentTypes, class TypedValueFunction>
			static UntypedOperation fromTypedValueFunction(const TypedValueFunction& typedValueFunction);

			UntypedExpression_t operator()(const Input &inputExpressions) const;

		private:
			UntypedOperation(UntypedExpressionFunction untypedExpressionFunction);

			UntypedExpressionFunction untypedExpressionFunction_;
		};
	}
}
