/*
@file
Compiler.h
Declares template class Compiler, which carries all the user-defined operators, functions, etc.

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
#include <type_traits>

#include "src/Utility/Typelist.h"

namespace metl
{
	namespace internal
	{
		template<class ExprT>
		class ConversionImpl
		{
			using FunctionType = std::function<ExprT(ExprT)>;
		public:
			ConversionImpl(FunctionType f) :f_(f) {}

			ExprT operator()(ExprT v) const 
			{
				auto resultExpression = f_(v);

				if (v.category() == CATEGORY::CONSTEXPR) return resultExpression.evaluatedExpression();

				return resultExpression;
			}

			template<class T>
			ConversionImpl& operator=(T&& t)
			{
				f_ = t;
				return *this;
			}

		private:

			FunctionType f_;
		};

		template<class ExprT, class From, class F>
		ConversionImpl<ExprT> makeCastImpl(const F& f) 
		{
			using To = std::result_of_t<F(From)>;

			static_assert(internal::isInList<From>(internal::getTypeList(Type<ExprT>())), "Type casted from is not one of the types of this compiler");
			static_assert(internal::isInList<To>(internal::getTypeList(Type<ExprT>())), "Type casted to is not one of the types of this compiler");

			auto impl = [f](const ExprT& from)
			{
				auto f_from = from.template get<From>();
				return ExprT(TypedExpression<To>{
					[f, f_from]()
					{
						return f(f_from());
					}
				});
			};

			return { impl };
		}

	}
}
