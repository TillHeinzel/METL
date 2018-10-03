// File: UntypedFunction.h
// 
// Copyright 2017-2018 Till Heinzel
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <vector>

#include "src/TypeErasure/UntypedOperation.h"

namespace metl
{
	namespace internal
	{
		template<class UntypedExpression_t>
		using UntypedFunction = UntypedOperation<UntypedExpression_t, std::vector<UntypedExpression_t>>;


		template <class UntypedExpression_t, class... ArgumentTypes, class TypedValueFunction>
		UntypedFunction<UntypedExpression_t> makeUntypedFunction(const TypedValueFunction& typedValueFunction)
		{
			return UntypedFunction<UntypedExpression_t>::template fromTypedValueFunction<ArgumentTypes...>(typedValueFunction);
		}
	}
}
