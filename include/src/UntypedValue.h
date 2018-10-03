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
#include "std17/remove_cvref.h"

namespace metl
{
	template<class... Ts>
	class UntypedVariable
	{
	public:
		template<class T>
		explicit UntypedVariable(T* var) : variable_(var)
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

		TYPE type() const
		{
			auto visitor = [](const auto* typedValue)
			{
				using T = std17::remove_cvref_t < std::remove_pointer_t<std17::remove_cvref_t<decltype(typedValue)>>>;

				return classToType2<T, Ts...>();
			};
			return mpark::visit(visitor, variable_);
		}

	private:
		mpark::variant<Ts*...> variable_;
	};

	template<class... Ts>
	class UntypedConstant
	{
	public:
		template<class T>
		explicit UntypedConstant(T val) : value_(val)
		{}

		template<class T>
		void setValue(const T& t)
		{
			value_ = t;
		}

		UntypedExpression<Ts...> makeUntypedExpression() const
		{
			auto visitor = [](const auto& typedValue)
			{
				return UntypedExpression<Ts...>::makeConstexpr(typedValue);
			};
			return mpark::visit(visitor, value_);
		}

		TYPE type() const
		{
			auto visitor = [](const auto& typedValue)
			{
				return classToType2<std17::remove_cvref_t<decltype(typedValue)>, Ts...>();
			};
			return mpark::visit(visitor, value_);
		}

	private:
		mpark::variant<Ts...> value_;
	};

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
