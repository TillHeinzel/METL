// File: Compiler.h
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

#include <map>

#include "src/TypeErasure/UntypedExpression.hpp"

#include "src/DataBase/CompilerEntityDataBase.hpp"

#include "Stack.hpp"

namespace metl
{
	namespace internal
	{

		template<class LiteralConverters, class... Ts>
		class Compiler
		{

		public:
			using Expression = UntypedExpression<Ts...>;
			constexpr static auto getTypeList() { return TypeList<Ts...>(); }

			Compiler(const LiteralConverters& literalConverters);


			template<class T>
			constexpr static TYPE type();

			Expression finish();

			void startAssignment(const std::string& varName);

		public:
			CompilerEntityDataBase<Ts...> bits_;
			LiteralConverters literalConverters_;
			Stack<Ts...> stack_;

		private:
			std::string assignToThis_;
		};

	}
}
