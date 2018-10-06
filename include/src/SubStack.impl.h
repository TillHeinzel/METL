// File: SubStack.impl.h
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
#include "SubStack.h"
#include "StackHelpers.h"
#include "Utility/MixedVisitor.h"

namespace metl
{
	namespace internal
	{
		template <class ... Ts>
		SubStack<Ts...>::SubStack(const CompilerEntityDataBase<Ts...>& bits
		)
			:bits_(bits),
			caster_(bits)
		{}

		template <class ... Ts>
		SubStack<Ts...>::SubStack(const CompilerEntityDataBase<Ts...>& bits, const std::string& FunctionName) :
			function_(FunctionName),
			bits_(bits),
			caster_(bits)
		{}

		template <class ... Ts>
		void SubStack<Ts...>::push(Expression l)
		{
			expressions_.push_back(l);
		}

		template <class ... Ts>
		void SubStack<Ts...>::push(const opCarrier& b)
		{
			if(b.associativity == ASSOCIATIVITY::LEFT)
			{
				while(operators_.size() > 0 && operators_.back().precedence <= b.precedence)
				{
					reduce();
				}
			}
			operators_.push_back(b);
		}

		template <class ... Ts>
		void SubStack<Ts...>::push(const suffixCarrier& suffix)
		{
			assert(!expressions_.empty());
			assert(expressions_.back().isConstexpr());

			const auto inType = expressions_.back().type();
			auto suffixImplOpt = bits_.findSuffix(mangleSuffix(suffix.name, {inType}));

			if(!suffixImplOpt)
			{
				auto targetType = caster_.findTypeForSuffix(suffix.name, inType);
				caster_.castTo(expressions_, {targetType});
				push(suffix);
			}

			const auto t = expressions_.back();
			expressions_.pop_back();

			auto resultExpression = suffixImplOpt->apply({t});

			expressions_.push_back(resultExpression);
		}

		template <class ... Ts>
		typename SubStack<Ts...>::Expression SubStack<Ts...>::finish()
		{
			if(function_)
			{
				evaluateFunction(*function_);
			}
			else
			{
				while(!operators_.empty())
				{
					reduce();
				}
			}
			assert(expressions_.size() == 1);

			const auto r = expressions_.back();
			expressions_.clear();
			return r;
		}


		template <class ... Ts>
		void SubStack<Ts...>::evaluateFunction(const std::string& functionName)
		{
			assert(operators_.empty());

			auto inTypes = getTypes(expressions_);

			auto functionOpt = bits_.findFunction(mangleName(functionName, inTypes));
			if(!functionOpt)
			{
				auto targetTypes = caster_.findTypesForFunction(functionName, inTypes);
				caster_.castTo(expressions_, targetTypes);
				evaluateFunction(functionName); // call recursively
			}

			auto resultExpression = functionOpt->apply(expressions_);

			expressions_.clear();
			expressions_.push_back(resultExpression);
		}

		template <class ... Ts>
		void SubStack<Ts...>::reduce()
		{
			assert(!operators_.empty());
			if(operators_.back().isUnary) reduceUnary();
			else reduceBinary();
		}

		template <class ... Ts>
		void SubStack<Ts...>::reduceBinary()
		{
			assert(expressions_.size() > 1);

			// get operator
			const auto opName = operators_.back().name;

			auto inTypes = std::vector<TYPE>{(expressions_.rbegin() + 1)->type(), expressions_.rbegin()->type()};
			auto operatorImplOpt = bits_.findOperator(mangleName(opName, inTypes));

			if(!operatorImplOpt)
			{
				auto targetTypes = caster_.findTypesForBinaryOperator(opName, inTypes);
				caster_.castTo(expressions_, targetTypes);
				reduce(); // call recursively
			}

			// get left and right expressions
			const auto r = expressions_.back();
			expressions_.pop_back();
			const auto l = expressions_.back();
			expressions_.pop_back();

			operators_.pop_back();

			auto resultExpression = operatorImplOpt->apply({l, r});


			expressions_.push_back(resultExpression);
		}

		template <class ... Ts>
		void SubStack<Ts...>::reduceUnary()
		{
			assert(expressions_.size() > 0u);

			// get operator
			const auto opName = operators_.back().name;

			auto inType = expressions_.back().type();
			auto operatorImplOpt = bits_.findOperator(mangleName(opName, {inType}));
			if(!operatorImplOpt)
			{
				auto targetType = caster_.findTypeForUnaryOperator(opName, inType);

				caster_.castTo(expressions_, {targetType});
				reduceUnary(); // call recursively
			}

			// get left and right expressions
			const auto t = expressions_.back();
			expressions_.pop_back();

			operators_.pop_back();

			auto resultExpression = operatorImplOpt->apply({t});

			expressions_.push_back(resultExpression);
		}



	}
}
