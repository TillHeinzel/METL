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

namespace metl
{
	template<class... Ts>
	class UntypedExpression
	{
	public:
		template<class T>
		static UntypedExpression makeConstexpr(T t)
		{
			static_assert(internal::isInList<T, Ts...>(), "cannot construct UntypedExpression with this type");
			auto ExpressionLambda = [t]()
			{
				return t;
			};
			auto typedExpression = TypedExpression<T>{ExpressionLambda};
			return UntypedExpression(typedExpression, true);
		}

		template<class T>
		static UntypedExpression makeNonConstexpr(const TypedExpression<T>& t)
		{
			return UntypedExpression(t, false);
		}

		template<class T>
		constexpr static bool canConstructTypedExpressionFrom = std::is_constructible<TypedExpression<typename std::result_of<T()>::type>, T>::value;

		template<class T>
		static UntypedExpression makeNonConstexpr(const T& t)
		{
			static_assert(canConstructTypedExpressionFrom<T>, "must be able to construct typedExpression from inputType");
			return makeNonConstexpr(TypedExpression<typename std::result_of<T()>::type>(t));
		}



		template<class T> TypedExpression<T> get() const
		{
			static_assert(internal::isInList<T, Ts...>(), "Error: Requested Type is not a valid type!");

			if(mpark::holds_alternative<TypedExpression<T>>(vals_)) return mpark::get<TypedExpression<T>>(vals_);
			throw std::runtime_error("this is not the correct type");
		}

		UntypedExpression evaluatedExpression() const
		{
			auto visitor = [](const auto& expr)
			{
				return UntypedExpression::makeConstexpr(expr());
			};

			return mpark::visit(visitor, vals_);
		}

		template<class T>
		bool isType() const
		{
			return type_ == toType<T>();
		}

		bool isConstexpr() const
		{
			return isConstexpr_;
		}

		TYPE type() const
		{
			return type_;
		}

		template<class T>
		constexpr static TYPE toType()
		{
			return classToType2<T, Ts...>();
		}
	private:

		template<class T, std::enable_if_t<!internal::isInList<T, Ts...>(), int> = 0>
		UntypedExpression(const TypedExpression<T>&, bool)
		{
			static_assert(internal::isInList<T, Ts...>(), "cannot construct UntypedExpression with this type");
		}

		template<class T, std::enable_if_t<internal::isInList<T, Ts...>(), int> = 0>
		UntypedExpression(const TypedExpression<T>& t, bool isConstexpr) :
			type_(classToType2<T, Ts...>()),
			isConstexpr_(isConstexpr),
			vals_(t)
		{}


		TYPE type_;
		bool isConstexpr_;

		mpark::variant<TypedExpression<Ts>...> vals_;
	};

	namespace internal
	{
		template<class... Ts>
		constexpr TypeList<Ts...> getTypeList(Type<UntypedExpression<Ts...>>)
		{
			return TypeList<Ts...>{};
		}
	}
}
