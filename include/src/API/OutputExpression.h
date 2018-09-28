/*
@file DynamicExpression.h
Defines class DynamicExpression, which is a variant-type to contain std::functions returning different values.
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

#include <map>
#include <string>

#include "src/DynamicExpression.h"
#include "src/StaticExpression.h"
#include "src/TypeEnum.h"

#include "src/CompilerBits.h"

namespace metl
{
	template<class... Ts>
	class OutputExpression
	{
	public:
		using Expression = DynamicExpression<Ts...>;

		OutputExpression(const Expression& expr, const std::map<std::string, internal::ConversionImpl<Expression>>& casts) :
			expressions_(castToAll(expr, casts)),
			type_(expr.type())
		{}

		template<class T>
		StaticExpression<T> get() const
		{
			auto it = expressions_.find(classToType2<T, Ts...>());
			if (it != expressions_.end())
			{
				return it->second.get<T>();
			}
			throw std::runtime_error("this is not the correct type");
		}

		template<class T>
		bool isType() const
		{
			return type_ == classToType2<T, Ts...>();
		}

	private:
		const std::map<TYPE, Expression> expressions_;
		TYPE type_;

		std::map<TYPE, Expression> castToAll(const Expression& expr, const std::map<std::string, internal::ConversionImpl<Expression>>& casts)
		{
			std::map<TYPE, Expression> castedExpressions{ {expr.type(), expr} };
			for (const auto& cast : casts)
			{
				try
				{
					auto castedExpression = cast.second(expr);
					castedExpressions.emplace(castedExpression.type(), std::move(castedExpression));
				}
				catch (const std::runtime_error&)
				{

				}
			}
			return castedExpressions;
		}
	};
}
