// File: VariableWrapper.h
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

#include "ThirdParty/Variant/variant.hpp"

#include "src/TypeErasure/UntypedExpression.h"

namespace metl
{
	template<class... Ts>
	class UntypedValue
	{
	public:
		template<class T>
		explicit UntypedValue(T* var): variable_(var)
		{}

		template<class T>
		void setValue(const T& t)
		{
			auto visitor = [&t](auto* typedVariablePtr)
			{
				*typedVariablePtr = t;
			};
			mpark::visit(visitor, variable_);
		}

		UntypedExpression<Ts...> makeUntypedExpression() const
		{
			auto visitor = [](const auto* typedVariablePtr)
			{
				auto valueLambda = [typedVariablePtr]()
				{
					return *typedVariablePtr;
				};

				return UntypedExpression<Ts...>::makeNonConstexpr(valueLambda);
			};
			return mpark::visit(visitor, variable_);
		}

	private:
		mpark::variant<Ts*...> variable_;

	};
}
