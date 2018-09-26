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
#include "Stack.h"

#include <cassert>

namespace metl
{
	namespace internal
	{
		template <class ... Ts>
		Stack<Ts...>::Stack(const CompilerBits<Ts...>& bits)
			:bits_(bits)
		{
			open();
		}

		template <class ... Ts>
		void Stack<Ts...>::push(const Expression& t)
		{
			assert(!subStacks_.empty());
			subStacks_.back().push(t);
		}

		template <class ... Ts>
		void Stack<Ts...>::pushFunction(std::string functionName)
		{
			open();
			subStacks_.back().pushFunction(functionName);
		}

		template <class ... Ts>
		void Stack<Ts...>::push(const opCarrier& op)
		{
			subStacks_.back().push(op);
		}

		template <class ... Ts>
		void Stack<Ts...>::push(const suffixCarrier& suffix)
		{
			subStacks_.back().push(suffix);
		}

		template <class ... Ts>
		void Stack<Ts...>::open()
		{
			subStacks_.emplace_back(bits_);
		}

		template <class ... Ts>
		void Stack<Ts...>::close()
		{
			assert(subStacks_.size() > 1);
			if (subStacks_.back().empty()) // special case for function taking zero arguments.
			{
				subStacks_.pop_back();
			}
			else
			{
				const auto r = subStacks_.back().finish();
				subStacks_.pop_back();
				subStacks_.back().push(r);
			}
		}

		template <class ... Ts>
		typename Stack<Ts...>::Expression Stack<Ts...>::finish()
		{
			assert(subStacks_.size() == 1);
			return subStacks_.back().finish();
		}

		template <class ... Ts>
		void Stack<Ts...>::clear()
		{
			subStacks_.clear();
			open();
		}
	}
}
