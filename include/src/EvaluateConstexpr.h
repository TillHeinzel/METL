/*
@file
Stack.impl.h
implementation of functions for template Stack, declared in Stack.fwd.h

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
#include "VarExpression.h"

namespace metl
{
	namespace internal
	{
		namespace detail
		{
			template<class Expression>
			Expression evaluateConstExpr_impl(TypeList<>, const Expression&)
			{
				throw std::runtime_error("can't evaluate constexpr, because type is not in typelist. However that happened...");
			}

			template<class T, class... Ts, class Expression>
			Expression evaluateConstExpr_impl(TypeList<T, Ts...>, const Expression& expression)
			{
				return expression.type() == Expression::template toType<T>() ?
					makeConstExpression<Expression>(expression.template get<T>()()) :
					evaluateConstExpr_impl(TypeList<Ts...>{}, expression);
			}

		}

		template<class... Ts>
		VarExpression<Ts...> evaluateConstExpr(const VarExpression<Ts...>& expression)
		{
			return detail::evaluateConstExpr_impl(TypeList<Ts...>{}, expression);
		}
	}
}
