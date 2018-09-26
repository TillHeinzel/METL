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
#include "SubStack.h"
#include "StackHelpers.h"

namespace metl
{
	namespace internal
	{
		template <class ... Ts>
		SubStack<Ts...>::SubStack(const CompilerBits<Ts...>& bits
			)
			:bits_(bits)
		{
		}

		template <class ... Ts>
		void SubStack<Ts...>::push(Expression l)
		{
			expressions_.push_back(l);
		}

		template <class ... Ts>
		void SubStack<Ts...>::push(const opCarrier& b)
		{
			if (b.associativity == ASSOCIATIVITY::LEFT)
			{
				while (operators_.size() > 0 && operators_.back().precedence <= b.precedence)
				{
					reduce();
				}
			}
			operators_.push_back(b);
		}

		template <class ... Ts>
		void SubStack<Ts...>::pushFunction(std::string FunctionName)
		{
			function_ = std::make_unique<std::string>(FunctionName);
		}

		template <class ... Ts>
		void SubStack<Ts...>::push(const suffixCarrier& suffix)
		{
			assert(!expressions_.empty());
			assert(expressions_.back().category() == CATEGORY::CONSTEXPR);

			const auto inType = expressions_.back().type();
			auto it = bits_.suffixImplementations_.find(mangleSuffix(suffix.name, { inType }));

			if (it == bits_.suffixImplementations_.end())
			{
				std::vector<TYPE> allowedCasts = bits_.castDeclarations_.at(inType);


				std::vector<TYPE> validCasts;
				std::vector<std::string> possibleFunctions;
				for (auto c : allowedCasts)
				{
					auto it2 = bits_.suffixImplementations_.find(mangleSuffix(suffix.name, { c }));
					if (it2 != bits_.suffixImplementations_.end())
					{
						possibleFunctions.push_back(it2->first);
						validCasts.push_back(c);
					}
				}
				// if we found multiple possible overloads that we can get through casts, this is an error
				if (possibleFunctions.size() > 1)
				{
					throw std::runtime_error("To many possible overloads for unary operator " + suffix.name);
				}

				// if we found a single possible overload that can be achieved through casting, use it!
				if (possibleFunctions.size() == 1)
				{
					castTo({ validCasts.back() });
					push(suffix); // call recursively
					return;
				}
				else
				{
					throw std::runtime_error("could not find a matching unary operator for " + suffix.name);
				}
			}

			// get left and right expressions
			const auto t = expressions_.back();
			expressions_.pop_back();

			auto resultExpression = it->second({ t });

			expressions_.push_back(resultExpression);
		}

		template <class ... Ts>
		typename SubStack<Ts...>::Expression SubStack<Ts...>::finish()
		{
			if (function_)
			{
				evaluateFunction();
			}
			else
			{
				while (!operators_.empty())
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
		void SubStack<Ts...>::evaluateFunction()
		{
			assert(operators_.empty());

			auto inTypes = getTypes(expressions_);

			auto it = bits_.functions_.find(mangleName(*function_, inTypes));
			if (it == bits_.functions_.end())
			{
				std::vector<std::vector<TYPE>> castCombis{ {} };
				for (auto t : inTypes)
				{
					castCombis = tensorSum(castCombis, bits_.castDeclarations_.at(t));
				}

				std::vector<std::vector<TYPE>> validCasts;
				std::vector<std::string> possibleFunctions;
				for (auto c : castCombis)
				{
					auto it2 = bits_.functions_.find(mangleName(*function_, c));
					if (it2 != bits_.functions_.end())
					{
						possibleFunctions.push_back(it2->first);
						validCasts.push_back(c);
					}
				}
				// if we found multiple possible overloads that we can get through casts, this is an error
				if (possibleFunctions.size() > 1)
				{
					throw std::runtime_error("To many possible overloads for function " + *function_);
				}

				// if we found a single possible overload that can be achieved through casting, use it!
				if (possibleFunctions.size() == 1)
				{
					castTo(validCasts.back());
					evaluateFunction(); // call recursively
					return;
				}
				else
				{
					throw std::runtime_error("could not find a matching function for " + *function_);
				}
			}

			auto resultExpression = it->second(expressions_);

			expressions_.clear();
			expressions_.push_back(resultExpression);
		}

		template <class ... Ts>
		void SubStack<Ts...>::reduce()
		{
			assert(!operators_.empty());
			if (operators_.back().isUnary) reduceUnary();
			else reduceBinary();
		}

		template <class ... Ts>
		void SubStack<Ts...>::reduceBinary()
		{
			assert(expressions_.size() > 1);

			// get operator
			const auto opName = operators_.back().name;

			auto inTypes = std::vector<TYPE>{ (expressions_.rbegin() + 1)->type(), expressions_.rbegin()->type() };
			auto it = bits_.operators_.find(mangleName(opName, inTypes));
			if (it == bits_.operators_.end())
			{
				std::vector<std::vector<TYPE>> castCombis{ {} };
				for (auto t : inTypes)
				{
					castCombis = tensorSum(castCombis, bits_.castDeclarations_.at(t));
				}

				std::vector<std::vector<TYPE>> validCasts;
				std::vector<std::string> possibleFunctions;
				for (auto c : castCombis)
				{
					auto it2 = bits_.operators_.find(mangleName(opName, c));
					if (it2 != bits_.operators_.end())
					{
						possibleFunctions.push_back(it2->first);
						validCasts.push_back(c);
					}
				}
				// if we found multiple possible overloads that we can get through casts, this is an error
				if (possibleFunctions.size() > 1)
				{
					throw std::runtime_error("To many possible overloads for operator " + opName);
				}

				// if we found a single possible overload that can be achieved through casting, use it!
				if (possibleFunctions.size() == 1)
				{
					castTo(validCasts.back());
					reduce(); // call recursively
					return;
				}
				else
				{
					throw std::runtime_error("could not find a matching operator for " + opName);
				}
			}

			// get left and right expressions
			const auto r = expressions_.back();
			expressions_.pop_back();
			const auto l = expressions_.back();
			expressions_.pop_back();

			operators_.pop_back();

			auto resultExpression = it->second({ l, r });


			expressions_.push_back(resultExpression);
		}

		template <class ... Ts>
		void SubStack<Ts...>::reduceUnary()
		{
			assert(expressions_.size() > 0u);

			// get operator
			const auto opName = operators_.back().name;

			auto inType = expressions_.back().type();
			auto it = bits_.operators_.find(mangleName(opName, { inType }));
			if (it == bits_.operators_.end())
			{
				std::vector<TYPE> allowedCasts = bits_.castDeclarations_.at(inType);


				std::vector<TYPE> validCasts;
				std::vector<std::string> possibleFunctions;
				for (auto c : allowedCasts)
				{
					auto it2 = bits_.operators_.find(mangleName(opName, { c }));
					if (it2 != bits_.operators_.end())
					{
						possibleFunctions.push_back(it2->first);
						validCasts.push_back(c);
					}
				}
				// if we found multiple possible overloads that we can get through casts, this is an error
				if (possibleFunctions.size() > 1)
				{
					throw std::runtime_error("To many possible overloads for unary operator " + opName);
				}

				// if we found a single possible overload that can be achieved through casting, use it!
				if (possibleFunctions.size() == 1)
				{
					castTo({ validCasts.back() });
					reduce(); // call recursively
					return;
				}
				else
				{
					throw std::runtime_error("could not find a matching unary operator for " + opName);
				}
			}

			// get left and right expressions
			const auto t = expressions_.back();
			expressions_.pop_back();

			operators_.pop_back();

			auto resultExpression = it->second({ t });

			expressions_.push_back(resultExpression);
		}

		template <class ... Ts>
		bool SubStack<Ts...>::are_all_const(const std::vector<Expression>& expressions)
		{
			for (const auto& e : expressions)
			{
				if (e.category() != CATEGORY::CONSTEXPR) return false;
			}
			return true;
		}

		template <class ... Ts>
		void SubStack<Ts...>::castTo(const std::vector<TYPE>& targetTypes)
		{
			auto i_target = 0u;
			// we cast only the last size(targetTypes) expressions. This then works for both functions and operators
			for (auto i = expressions_.size() - targetTypes.size(); i < expressions_.size(); ++i)
			{
				auto& expr = expressions_.at(i);
				const auto fromType = expr.type();
				const auto toType = targetTypes.at(i_target);
				if (fromType != toType)
				{
					expr = bits_.castImplementations_.at(mangleCast(fromType, toType))(expr);
				}
				++i_target;
			}
		}
	}
}