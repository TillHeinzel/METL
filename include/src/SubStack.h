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

#include <string>

#include "ThirdParty/Variant/variant.hpp"

#include "src/TypeErasure/UntypedExpression.h"

#include "src/opCarrier.h"
#include "src/suffixCarrier.h"
#include "src/CompilerEntityDataBase.h"

#include "src/FunctionSubStack.h"
#include "src/ExpressionSubStack.h"

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

			explicit SubStack(const CompilerEntityDataBase<Ts...>& bits, const std::string& FunctionName);

			void push(Expression l);
			void push(const opCarrier& b);
			void push(const suffixCarrier& suffix);

			Expression finish();

			bool empty() const;
		private:
			mpark::variant<ExpressionSubStack<Ts...>, FunctionSubStack<Ts...>> specificSubStack_;
		};
	}
}
