// File: Compiler.impl.h
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
#include "src/Compiler.hpp"

#include <tao/pegtl.hpp>

#include "grammar.hpp"
#include "actions.hpp"

#include "nameMangling.hpp"
#include "Stack.impl.hpp"

#include "src/VariableExpression.hpp"

namespace metl
{
	namespace internal
	{
		template <class LiteralConverters, class ... Ts>
		Compiler<LiteralConverters, Ts...>::Compiler(const LiteralConverters& literalConverters) :
			literalConverters_(literalConverters),
			stack_(bits_)
		{}

		template <class LiteralConverters, class... Ts>
		template <class T>
		constexpr TYPE Compiler<LiteralConverters, Ts...>::type()
		{
			return classToType2<T, Ts...>();
		}

		namespace
		{


			template<class Expression>
			void assignToVar_impl(Expression&, const Expression&)// assumes the two expression have the same type
			{
				throw std::runtime_error("Something weird happened here");
			}

			template<class T, class... RemainingTs, class Expression>
			void assignToVar_impl(Expression& existingVarExpression, const Expression& evaluatedExpr)// assumes the two expression have the same type
			{
				if(existingVarExpression.type() == existingVarExpression.template toType<T>())
				{
					*existingVarExpression.template get<T>().template target<VariableExpression<T>>()->v = evaluatedExpr.template get<T>()();
				}
				else
				{
					assignToVar_impl<RemainingTs...>(existingVarExpression, evaluatedExpr);
				}
			}

			template<class... Ts>
			void assignToVar(UntypedExpression<Ts...>& existingVarExpression, const UntypedExpression<Ts...>& evaluatedExpr)// assumes the two expression have the same type
			{
				assignToVar_impl<Ts...>(existingVarExpression, evaluatedExpr);
			}
		}

		template <class LiteralConverters, class ... Ts>
		typename Compiler<LiteralConverters, Ts...>::Expression Compiler<LiteralConverters, Ts...>::finish()
		{
			auto expr = stack_.finish();

			if(assignToThis_ != "")
			{
				auto untypedResult = expr.evaluateUntyped();
				assert(untypedResult.isConstant());
				assert(untypedResult.type() == expr.type());

				auto valueOpt = bits_.findValue(assignToThis_);
				if(!valueOpt)
				{
					bits_.addConstantOrVariable(assignToThis_, untypedResult);
				}
				else if(valueOpt->isConstant())
				{
					bits_.addConstantOrVariable(assignToThis_, untypedResult);
				}
				else
				{
					if(valueOpt->type() == untypedResult.type())
					{
						valueOpt->setValueUntyped(untypedResult);
					}
					else
					{
						auto castOpt = bits_.findCast(mangleCast(expr.type(), valueOpt->type()));
						if(castOpt)
						{
							auto castedResult = castOpt->apply(expr).evaluateUntyped();
							valueOpt->setValueUntyped(castedResult);
						}
						else
						{
							throw std::runtime_error("cannot assign different types without available cast");
						}
					}
				}
			}

			assignToThis_ = "";
			return expr;
		}

		template <class LiteralConverters, class ... Ts>
		void Compiler<LiteralConverters, Ts...>::startAssignment(const std::string& varName)
		{
			assignToThis_ = varName;
		}
	}
}