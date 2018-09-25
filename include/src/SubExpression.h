/*
@file SubExpression.h

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

#include "src/Utility/DeepCopy.h"

#include "src/Utility/TypeList.h"
#include "src/Utility/DeepCopy.h"

#include "TypeEnum.h"
#include "CategoryEnum.h"
#include "ExpressionType.h"


namespace metl
{
	template<class... Ts>
	class SubExpression
	{
	public:
		template<class T>
		SubExpression(const exprType<T>& t, CATEGORY category = CATEGORY::DYNEXPR) :
			type_(classToType2<T, Ts...>()),
			category_(category),
			vals_(t)
		{
		}

		template<class T> exprType<T> get() const
		{
			constexpr auto index = internal::findFirstIndex<T>(internal::TypeList<Ts...>{});
			static_assert(index < sizeof...(Ts), "Error: Requested Type is not a valid type!");

			if (mpark::holds_alternative<exprType<T>>(vals_)) return mpark::get<exprType<T>>(vals_);
			throw std::runtime_error("this is not the correct type");
		}

		TYPE type() const { return type_; }
		CATEGORY category() const { return category_; }

		template<class T>
		constexpr static TYPE toType() { return classToType2<T, Ts...>(); }
	private:
		TYPE type_;
		CATEGORY category_;

		mpark::variant<exprType<Ts>...> vals_;
	};
}
