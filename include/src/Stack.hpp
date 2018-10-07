// File: Stack.h
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
#include <string>
#include <vector>

#include "src/TypeErasure/UntypedExpression.hpp"

#include "BasicOperatorData.hpp"
#include "SuffixID.hpp"
#include "CompilerEntityDataBase.hpp"
#include "SubStack.hpp"


namespace metl
{

	namespace internal {
		template<class... Ts>
		class Stack
		{
			using Expression = UntypedExpression<Ts...>;

		public:
			explicit Stack(const CompilerEntityDataBase<Ts...>& bits);

			void push(const Expression& t);

			void pushFunction(std::string functionName);

			void push(const BasicOperatorData& op);

			void push(const SuffixID& suffix);

			void open();

			void close();

			Expression finish();
			void clear();

		private:
			std::vector<SubStack<Ts...>> subStacks_;

			const CompilerEntityDataBase<Ts...>& bits_;
		};
	}
}
