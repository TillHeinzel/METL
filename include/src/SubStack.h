/*
@file
Stack.fwd.h
Declares class Stack, which implements the shunting-yard algorithm.

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
#include <string>

#include "ThirdParty/Optional/optional.hpp"

#include "src/TypeErasure/UntypedExpression.h"

#include "opCarrier.h"
#include "suffixCarrier.h"
#include "CompilerEntityDataBase.h"

namespace metl
{
	namespace internal
	{
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

			bool empty() const { return expressions_.empty(); }
		private:
			void evaluateFunction(const std::string& functionName);

			void reduce();
			void reduceBinary();
			void reduceUnary();

			bool plannedSignSwitch = false;

			std::vector<Expression> expressions_;
			std::vector<opCarrier> operators_;

			tl::optional<std::string> function_;

			const CompilerEntityDataBase<Ts...>& bits_;


		private:
			void castTo(const std::vector<TYPE>& targetTypes);
		};
	}
}
