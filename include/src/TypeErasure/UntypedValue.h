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

#include "src/TypeErasure/UntypedConstant.h"
#include "src/TypeErasure/UntypedVariable.h"

namespace metl
{
	template<class... Ts>
	class UntypedValue
	{
	public:
		template<class T>
		explicit UntypedValue(T val): value_(UntypedConstant<Ts...>(val))
		{}

		template<class T>
		explicit UntypedValue(T* val): value_(UntypedVariable<Ts...>(val))
		{}

		template<class T>
		void setValue(const T& t)
		{
			auto visitor = [&t](auto& constantOrVariable)
			{
				constantOrVariable.setValue(t);
			};
			mpark::visit(visitor, value_);
		}

		UntypedExpression<Ts...> makeUntypedExpression() const
		{
			auto visitor = [](const auto& constantOrVariable)
			{
				return constantOrVariable.makeUntypedExpression();
			};
			return mpark::visit(visitor, value_);
		}

		TYPE type() const
		{
			auto visitor = [](const auto& constantOrVariable)
			{
				return constantOrVariable.type();
			};
			return mpark::visit(visitor, value_);
		}
		bool isConstant()
		{
			return mpark::holds_alternative<UntypedConstant<Ts...>>(value_);
		}

		bool isVariable()
		{
			return mpark::holds_alternative<UntypedVariable<Ts...>>(value_);
		}

	private:
		mpark::variant<UntypedConstant<Ts...>, UntypedVariable<Ts...>> value_;

	};
}
