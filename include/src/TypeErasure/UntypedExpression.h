/*
@file UntypedExpression.h
Defines class UntypedExpression, which is a variant-type to contain std::functions returning different values.
This is used to contain the results of parsing.

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

#include "ThirdParty/Variant/variant.hpp"

#include "src/Utility/TypeList.h"
#include "src/std17/remove_cvref.h"

#include "src/TypeErasure/TypeEnum.h"
#include "src/TypeErasure/TypedExpression.h"

#include "src/TypeErasure/CategoryEnum.h"

namespace metl
{
	template<class... Ts>
	class UntypedExpression
	{
	public:
		template<class T>
		static UntypedExpression makeConstexpr(T&& t)
		{
			auto ExpressionLambda = [t = std::forward<T>(t)]()
			{
				return t;
			};
			return UntypedExpression(std::move(ExpressionLambda), CATEGORY::CONSTEXPR);
		}

		template<class T>
		static UntypedExpression makeNonConstexpr(const TypedExpression<T>& t)
		{
			return UntypedExpression(t, CATEGORY::DYNEXPR);
		}

		template<class T, std::enable_if_t<!internal::isInList<T, Ts...>(), int> = 0>
		UntypedExpression(const TypedExpression<T>& t, CATEGORY category = CATEGORY::DYNEXPR)
		{
			static_assert(!internal::isInList<T, Ts...>(), "cannot construct UntypedExpression with this type");
		}

		template<class T, std::enable_if_t<internal::isInList<T, Ts...>(), int> = 0>
		UntypedExpression(const TypedExpression<T>& t, CATEGORY category = CATEGORY::DYNEXPR) :
			type_(classToType2<T, Ts...>()),
			category_(category),
			vals_(t)
		{}

		template<class T> TypedExpression<T> get() const
		{
			constexpr auto index = internal::findFirstIndex<T>(internal::TypeList<Ts...>{});
			static_assert(index < sizeof...(Ts), "Error: Requested Type is not a valid type!");

			if(mpark::holds_alternative<TypedExpression<T>>(vals_)) return mpark::get<TypedExpression<T>>(vals_);
			throw std::runtime_error("this is not the correct type");
		}

		UntypedExpression evaluatedExpression() const
		{
			auto visitor = [](const auto& expr)
			{
				using exprType_T = std17::remove_cvref_t<decltype(expr)>;
				auto value = expr();
				auto constExpr = exprType_T([value]()
				{
					return value;
				});
				return UntypedExpression<Ts...>(constExpr, CATEGORY::CONSTEXPR);
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
			return category_ == CATEGORY::CONSTEXPR;
		}

		TYPE type() const
		{
			return type_;
		}
		CATEGORY category() const
		{
			return category_;
		}

		template<class T>
		constexpr static TYPE toType()
		{
			return classToType2<T, Ts...>();
		}
	private:
		TYPE type_;
		CATEGORY category_;

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