/*
@file ExpressionType.h

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

#include "../std17/apply.h"

#include "../Utility/evaluate_each.h"

namespace metl
{
	template<class T> using TypedExpression = std::function<T()>;

	namespace internal
	{
		template<class TypedFunction, class TypedArgumentExpressions>
		auto makeTypedExpressionLambda(TypedFunction&& typedFunction, TypedArgumentExpressions&& typedArgumentExpressions)
		{
			return
				[typedFunction = std::forward<TypedFunction>(typedFunction),
				typedArgumentExpressions = std::forward<TypedArgumentExpressions>(typedArgumentExpressions)]
			()
			{
				auto typedArguments = evaluate_each(typedArgumentExpressions);
				return std17::apply(typedFunction, std::move(typedArguments));
			};
		}

		template<class TypedFunction, class TypedArgumentExpressions>
		auto makeTypedExpression(TypedFunction&& typedFunction, TypedArgumentExpressions&& typedArgumentExpressions)
		{
			auto typedExpressionLambda = makeTypedExpressionLambda(
				std::forward<TypedFunction>(typedFunction),
				std::forward<TypedArgumentExpressions>(typedArgumentExpressions)
				);

			using RetType = decltype(typedExpressionLambda());
			return TypedExpression<RetType>(typedExpressionLambda);
		}
	}
}

//#include <type_traits>
//
//namespace metl
//{
//	template<class T>
//	class TypedExpression
//	{
//		using FunctionType = std::function<T()>;
//	public:
//
//		template<class F, typename = std::enable_if_t<!std::is_constructible<FunctionType, const F&>::value>>
//		TypedExpression(const F& f, ...)
//		{}
//
//		template<class F, typename = std::enable_if_t<std::is_constructible<FunctionType, const F&>::value>>
//		TypedExpression(const F& f) :
//			f_(f)
//		{}
//
//		T operator() () const
//		{
//			return f_();
//		}
//
//		template<class... TT>
//		auto* target()
//		{
//			return f_.target<TT...>();
//		}
//	private:
//		FunctionType f_;
//	};
//}
//
