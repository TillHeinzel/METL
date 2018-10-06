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
		SubStack<Ts...>::SubStack(const CompilerEntityDataBase<Ts...>& bits) :
			specificSubStack_(ExpressionSubStack<Ts...>(bits))
		{}

		template <class ... Ts>
		SubStack<Ts...>::SubStack(const CompilerEntityDataBase<Ts...>& bits, const std::string& FunctionName) :
			specificSubStack_(FunctionSubStack<Ts...>(bits, FunctionName))
		{}

		template <class ... Ts>
		void SubStack<Ts...>::push(Expression l)
		{
			auto visitor = [&l](auto& subStack)
			{
				subStack.push(l);
			};
			mpark::visit(visitor, specificSubStack_);
		}

		template <class ... Ts>
		void SubStack<Ts...>::push(const opCarrier& b)
		{
			struct Visitor
			{
				const opCarrier& b;

				void operator() (FunctionSubStack<Ts...>&)
				{
					assert(false);
				}

				void operator() (ExpressionSubStack<Ts...>& subStack)
				{
					subStack.push(b);
				}
			};

			mpark::visit(Visitor{b}, specificSubStack_);
		}

		template <class ... Ts>
		void SubStack<Ts...>::push(const suffixCarrier& suffix)
		{
			struct Visitor
			{
				const suffixCarrier& s;

				void operator() (FunctionSubStack<Ts...>&)
				{
					assert(false);
				}

				void operator() (ExpressionSubStack<Ts...>& subStack)
				{
					subStack.push(s);
				}
			};

			mpark::visit(Visitor{suffix}, specificSubStack_);
		}

		template <class ... Ts>
		typename SubStack<Ts...>::Expression SubStack<Ts...>::finish()
		{
			auto visitor = [](auto& subStack)
			{
				return subStack.finish();
			};
			return mpark::visit(visitor, specificSubStack_);
		}

		template <class ... Ts>
		bool SubStack<Ts...>::empty() const
		{
			auto visitor = [](auto& subStack)
			{
				return subStack.empty();
			};
			return mpark::visit(visitor, specificSubStack_);
		}
	}
}
