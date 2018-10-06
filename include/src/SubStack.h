// File: SubStack.h
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

#include <vector>
#include <string>

#include "ThirdParty/Optional/optional.hpp"
#include "ThirdParty/Variant/variant.hpp"

#include "src/TypeErasure/UntypedExpression.h"

#include "opCarrier.h"
#include "suffixCarrier.h"
#include "CompilerEntityDataBase.h"

namespace metl
{
	namespace internal
	{
		//template<class... Ts>
		//class FunctionSubStack
		//{
		//	using Expression = UntypedExpression<Ts...>;
		//public:
		//	FunctionSubStack(const CompilerEntityDataBase<Ts...>& database, std::string functionName) :
		//		functionName_(std::move(functionName)),
		//		dataBase_(database)
		//	{}

		//	void push(Expression l)
		//	{
		//		arguments_.push_back(std::move(l));
		//	}

		//private:
		//	const CompilerEntityDataBase<Ts...>& dataBase_;

		//	const std::string functionName_;
		//	std::vector<Expression> arguments_;
		//};

		//template<class... Ts>
		//class ExpressionSubStack
		//{
		//	using Expression = UntypedExpression<Ts...>;
		//public:
		//	explicit ExpressionSubStack(const CompilerEntityDataBase<Ts...>& dataBase):dataBase_(dataBase){}

		//	void push(Expression l)
		//	{
		//		subExpressions_.push_back(std::move(l));
		//	}
		//	void push(const opCarrier& b)
		//	{
		//		if(b.associativity == ASSOCIATIVITY::LEFT)
		//		{
		//			while(operators_.size() > 0 && operators_.back().precedence <= b.precedence)
		//			{
		//				reduce();
		//			}
		//		}
		//	}
		//	void push(const suffixCarrier& suffix)
		//	{

		//		assert(!subExpressions_.empty());
		//		assert(subExpressions_.back().isConstexpr());

		//		const auto inType = subExpressions_.back().type();
		//		auto suffixImplOpt = dataBase_.findSuffix(mangleSuffix(suffix.name, {inType}));

		//		if(!suffixImplOpt)
		//		{
		//			std::vector<TYPE> allowedCasts = dataBase_.getAllTypesCastableFrom(inType);


		//			std::vector<TYPE> validCasts;
		//			std::vector<std::string> possibleFunctions;
		//			for(auto c : allowedCasts)
		//			{
		//				auto castedSuffixName = mangleSuffix(suffix.name, {c});
		//				auto castedSuffixImplOpt = dataBase_.findSuffix(castedSuffixName);
		//				if(castedSuffixImplOpt)
		//				{
		//					possibleFunctions.push_back(castedSuffixName);
		//					validCasts.push_back(c);
		//				}
		//			}
		//			// if we found a single possible overload that can be achieved through casting, use it!
		//			if(possibleFunctions.size() == 1)
		//			{
		//				castTo({validCasts.back()});
		//				push(suffix); // call recursively
		//				return;
		//			}
		//			// if we found multiple possible overloads that we can get through casts, this is an error
		//			if(possibleFunctions.size() > 1)
		//			{
		//				throw std::runtime_error("To many possible overloads for unary operator " + suffix.name);
		//			}

		//			else
		//			{
		//				throw std::runtime_error("could not find a matching unary operator for " + suffix.name);
		//			}
		//		}

		//		const auto t = subExpressions_.back();
		//		subExpressions_.pop_back();

		//		auto resultExpression = suffixImplOpt->apply({t});

		//		subExpressions_.push_back(resultExpression);
		//	}

		//private:
		//	std::vector<Expression> subExpressions_;
		//	std::vector<opCarrier> operators_;

		//	const CompilerEntityDataBase<Ts...>& dataBase_;


		//	void reduce();
		//	void reduceBinary();
		//	void reduceUnary();
		//};

		template<class... Ts>
		class SubStack
		{
			using Expression = UntypedExpression<Ts...>;

		public:

			explicit SubStack(const CompilerEntityDataBase<Ts...>& bits);


			void push(const Expression l);
			void push(const opCarrier& b);
			void pushFunction(std::string FunctionName);
			void push(const suffixCarrier& suffix);

			Expression finish();

			bool empty() const
			{
				return expressions_.empty();
			}
		private:
			TYPE findTypeForSuffix(const std::string& opName, const TYPE inType) const;

			void evaluateFunction(const std::string& functionName);


			std::vector<TYPE> findTypesForFunction(const std::string& functionName, const std::vector<TYPE>& inTypes) const;

			void reduce();
			void reduceBinary();

			std::vector<TYPE> findTypesForBinaryOperator(const std::string& opName, const std::vector<TYPE>& inTypes) const;

			void reduceUnary();

			TYPE findTypeForUnaryOperator(const std::string& opName, const TYPE inType) const;

			bool plannedSignSwitch = false;

			std::vector<Expression> expressions_;
			std::vector<opCarrier> operators_;

			tl::optional<std::string> function_;

			//mpark::variant<ExpressionSubStack<Ts...>, FunctionSubStack<Ts...>> specificSubStack_;

			const CompilerEntityDataBase<Ts...>& bits_;
		};
	}
}
