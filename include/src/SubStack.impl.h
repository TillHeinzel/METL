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
			:bits_(bits)
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
		void SubStack<Ts...>::pushFunction(std::string functionName)
		{
			function_ = functionName;
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
				auto targetType = findTypeForSuffix(suffix.name, inType);
				bits_.castTo(expressions_, {targetType});
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
				auto targetTypes = findTypesForFunction(functionName, inTypes);
				bits_.castTo(expressions_, targetTypes);
				evaluateFunction(functionName); // call recursively
			}

			auto resultExpression = functionOpt->apply(expressions_);

			expressions_.clear();
			expressions_.push_back(resultExpression);
		}

		template <class ... Ts>
		template <class CheckExistence>
		std::vector<std::vector<TYPE>> SubStack<Ts...>::getValidCasts(const std::vector<TYPE> inTypes,
			const CheckExistence& doTypesWork) const
		{

			std::vector<std::vector<TYPE>> castCombis{{}};
			for(auto t : inTypes)
			{
				castCombis = tensorSum(castCombis, bits_.getAllTypesCastableFrom(t));
			}

			std::vector<std::vector<TYPE>> validCasts;
			for(auto c : castCombis)
			{
				if(doTypesWork(c))
				{
					validCasts.push_back(c);
				}
			}

			return validCasts;
		}

		template<class T>
		void checkExactlyOneValidCast(const std::vector<T>& validCasts, const std::string& objectLabel)
		{
			if(validCasts.size() > 1)
			{
				throw std::runtime_error("To many possible overloads for " + objectLabel);
			}

			if(validCasts.size() == 0)
			{
				throw std::runtime_error("could not find a match for " + objectLabel);
			}
		}

		template <class ... Ts>
		std::vector<TYPE> SubStack<Ts...>::findTypesForFunction(const std::string& functionName, const std::vector<TYPE>& inTypes) const
		{
			auto doTypesWork = [&functionName, &database = bits_](const std::vector<TYPE>& toTypes) -> bool
			{
				auto mangledName = mangleName(functionName, toTypes);
				auto castedImplOpt = database.findFunction(mangledName);

				return castedImplOpt.has_value();
			};

			auto validCasts = getValidCasts(inTypes, doTypesWork);

			checkExactlyOneValidCast(validCasts, "function " + functionName);
			return validCasts.back();
			
		}

		template <class ... Ts>
		std::vector<TYPE> SubStack<Ts...>::findTypesForBinaryOperator(const std::string& opName, const std::vector<TYPE>& inTypes) const
		{
			auto doTypesWork = [&opName, &database = bits_](const std::vector<TYPE>& toTypes) -> bool
			{
				auto castedName = mangleName(opName, toTypes);
				auto castedImplOpt = database.findOperator(castedName);

				return castedImplOpt.has_value();
			};

			auto validCasts = getValidCasts(inTypes, doTypesWork);

			checkExactlyOneValidCast(validCasts, "operator " + opName);
			return validCasts.back();
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
				auto targetTypes = findTypesForBinaryOperator(opName, inTypes);
				bits_.castTo(expressions_, targetTypes);
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
				auto targetType = findTypeForUnaryOperator(opName, inType);

				bits_.castTo(expressions_, {targetType});
				reduceUnary(); // call recursively
			}

			// get left and right expressions
			const auto t = expressions_.back();
			expressions_.pop_back();

			operators_.pop_back();

			auto resultExpression = operatorImplOpt->apply({t});

			expressions_.push_back(resultExpression);
		}


		template <class ... Ts>
		template <class CheckExistence>
		std::vector<TYPE> SubStack<Ts...>::getValidCasts(const TYPE inType, const CheckExistence& doesTypeWork) const
		{
			std::vector<TYPE> allowedCasts = bits_.getAllTypesCastableFrom(inType);

			std::vector<TYPE> validCasts;
			for(auto c : allowedCasts)
			{
				if(doesTypeWork(c))
				{
					validCasts.push_back(c);
				}
			}

			return validCasts;
		}


		template <class ... Ts>
		TYPE SubStack<Ts...>::findTypeForSuffix(const std::string& suffix, const TYPE inType) const
		{
			auto doesTypeWork = [&suffix, &database = bits_](TYPE t) -> bool
			{
				auto castedName = mangleSuffix(suffix, {t});
				auto castedImplOpt = database.findSuffix(castedName);

				return castedImplOpt.has_value();
			};

			auto validCasts = getValidCasts(inType, doesTypeWork);

			checkExactlyOneValidCast(validCasts, "suffix " + suffix);
			return validCasts.back();
		}

		template <class ... Ts>
		TYPE SubStack<Ts...>::findTypeForUnaryOperator(const std::string& opName, const TYPE inType) const
		{
			auto doesTypeWork = [&opName, &database = bits_](TYPE t) -> bool
			{
				auto castedName = mangleName(opName, {t});
				auto castedImplOpt = database.findOperator(castedName);

				return castedImplOpt.has_value();
			};

			auto validCasts = getValidCasts(inType, doesTypeWork);

			checkExactlyOneValidCast(validCasts, "unary operator " + opName);
			return validCasts.back();
		}

	}
}
