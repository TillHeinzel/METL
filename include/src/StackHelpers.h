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

#include <vector>

#include "TypeEnum.h"

namespace metl
{
	namespace internal
	{
		namespace
		{
			template<class Expression>
			std::vector<TYPE> getTypes(const std::vector<Expression>& expressions)
			{
				std::vector<TYPE> types{};
				for (const auto& expr : expressions)
				{
					types.push_back(expr.type());
				}
				return types;
			}

			inline std::vector<TYPE> addThis(std::vector<TYPE> left, TYPE right)
			{
				left.push_back(right);
				return left;
			}

			inline std::vector<std::vector<TYPE>> tensorSum(const std::vector<std::vector<TYPE>>& left, const std::vector<TYPE>& right)
			{
				std::vector<std::vector<TYPE>> retval{};
				for (const auto& x : left)
				{
					for (const auto& y : right)
					{
						retval.push_back(addThis(x, y));
					}
				}

				return retval;
			}
		}
	}
}

namespace metl
{
	namespace internal
	{
		template<class Expression>
		Expression changeSign_impl(TypeList<>, const Expression&)
		{
			throw std::runtime_error("can't change sign, because type is not in typelist. However that happened...");
		}

		template<class T, class... Ts, class Expression>
		Expression changeSign_impl(TypeList<T, Ts...>, const Expression& expression)
		{
			return expression.type() == Expression::template toType<T>() ?
				Expression(exprType<T>([f{ expression.template get<T>() }](){return -f(); })) :
				changeSign_impl(TypeList<Ts...>{}, expression);
		}

		template<class... Ts>
		VarExpression<Ts...> changeSign(const VarExpression<Ts...>& expression)
		{
			return changeSign_impl(TypeList<Ts...>{}, expression);
		}
	}
}
