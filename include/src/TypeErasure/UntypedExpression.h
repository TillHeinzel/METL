// File: UntypedExpression.h
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

#include "src/Utility/TypeList.h"

#include "src/TypeErasure/TypeEnum.h"
#include "src/TypeErasure/TypedExpression.h"

#include "src/TypeErasure/UntypedValue.fwd.h"

namespace metl
{
	template<class... Ts>
	class UntypedExpression
	{
	public:
		template<class T>
		static UntypedExpression makeConstexpr(T t);

		template<class T>
		static UntypedExpression makeNonConstexpr(const TypedExpression<T>& t);

		template<class T>
		constexpr static bool canConstructTypedExpressionFrom = std::is_constructible<TypedExpression<typename std::result_of<T()>::type>, T>::value;

		template<class T>
		static UntypedExpression makeNonConstexpr(const T& t);



		template<class T>
		TypedExpression<T> get() const;

		UntypedExpression evaluatedExpression() const;

		UntypedValue<Ts...> evaluateUntyped() const;

		template<class T>
		bool isType() const;

		bool isConstexpr() const;

		TYPE type() const;

		template<class T>
		constexpr static TYPE toType();
	private:

		template<class T, std::enable_if_t<!internal::isInList<T, Ts...>(), int> = 0>
		UntypedExpression(const TypedExpression<T>&, bool);

		template<class T, std::enable_if_t<internal::isInList<T, Ts...>(), int> = 0>
		UntypedExpression(const TypedExpression<T>& t, bool isConstexpr);


		TYPE type_;
		bool isConstexpr_;

		mpark::variant<TypedExpression<Ts>...> vals_;
	};

	template <class ... Ts>
	template <class T>
	constexpr TYPE UntypedExpression<Ts...>::toType()
	{
		return classToType2<T, Ts...>();
	}


	namespace internal
	{
		template<class... Ts>
		constexpr TypeList<Ts...> getTypeList(Type<UntypedExpression<Ts...>>)
		{
			return TypeList<Ts...>{};
		}
	}
}
