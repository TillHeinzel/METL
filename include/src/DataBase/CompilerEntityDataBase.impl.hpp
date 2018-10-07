// File: CompilerEntityDataBase.impl.h
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
#include "src/DataBase/CompilerEntityDataBase.hpp"

#include <tao/pegtl.hpp>

#include "src/Utility/insert_or_emplace.hpp"

#include "src/grammar.hpp"
#include "src/actions.hpp"

#include "src/nameMangling.hpp"
#include "src/Stack.impl.hpp"


namespace metl
{
	namespace internal
	{
		template <class ... Ts>
		void CompilerEntityDataBase<Ts...>::setOperatorPrecedence(const std::string& op, unsigned precedence,
			ASSOCIATIVITY associativity)
		{

			// if operator already exists, replace precedence. Else, insert new operator with new precedence

			internal::insert_or_emplace(opCarriers_, op, BasicOperatorData{ op, precedence, associativity, false });
		}

		template <class ... Ts>
		void CompilerEntityDataBase<Ts...>::setUnaryOperatorPrecedence(const std::string& op, unsigned precedence)
		{
			internal::insert_or_emplace(unaryCarriers_, op, BasicOperatorData{ op, precedence, ASSOCIATIVITY::RIGHT, true });
		}

		template <class ... Ts>
		void CompilerEntityDataBase<Ts...>::setOperator(const std::string& token,
			const std::vector<TYPE>& paramTypes, const UntypedFunction<Expression>& op)
		{
			// check, if operator is in the list of precedences. 
			auto it = opCarriers_.find(token);
			if (it == opCarriers_.end())
			{
				throw std::runtime_error("The operator \" " + token + "\" has no precedence set!");
			}

			auto mangledName = mangleName(token, paramTypes);
			insert_or_emplace(operators_, mangledName, op);
		}

		template <class ... Ts>
		void CompilerEntityDataBase<Ts...>::setUnaryOperator(const std::string& token, TYPE paramType,
			const UntypedFunction<Expression>& op)
		{
			auto it = unaryCarriers_.find(token);
			if (it == unaryCarriers_.end())
			{
				throw std::runtime_error("The unary operator \" " + token + "\" has no precedence set!");
			}

			auto mangledName = mangleName(token, { paramType });
			insert_or_emplace(operators_, mangledName, op);
		}

		template <class ... Ts>
		void CompilerEntityDataBase<Ts...>::setFunction(const std::string& token,
			const std::vector<TYPE>& paramTypes, const UntypedFunction<Expression>& function)
		{
			insert_or_emplace(functionNames_, token, token);
			insert_or_emplace(functions_, mangleName(token, paramTypes), function);
		}

		template <class ... Ts>
		void CompilerEntityDataBase<Ts...>::setCast(const TYPE from, const TYPE to, const UntypedConversion<Expression>& fs)
		{
			auto it = castDeclarations_.find(from);
			if (it == castDeclarations_.end())
			{
				castDeclarations_.emplace(from, std::vector<TYPE>{ to });
			}
			else
			{
				it->second.push_back(to);
			}

			insert_or_emplace(castImplementations_, mangleCast(from, to), fs);
		}

		template <class ... Ts>
		void CompilerEntityDataBase<Ts...>::setSuffix(const std::string& token, const TYPE from, const UntypedConversion<Expression>& conversion)
		{
			insert_or_emplace(suffixes_, token, SuffixID{ token });
			insert_or_emplace(suffixImplementations_, mangleSuffix(token, from), conversion);
		}

		inline bool isAllAlnum(const std::string& s)
		{
			for(const auto& c:s)
			{
				if (!isalnum(c)) return false;
			}
			return true;
		}

		template <class ... Ts>
		void CompilerEntityDataBase<Ts...>::addConstantOrVariable(const std::string& token,	const UntypedValue<Ts...>& val)
		{
			if(token.empty()) throw std::runtime_error("token must not be empty!");
			if(!isAllAlnum(token)) throw std::runtime_error("token must be alphanumeric!");
			if(isdigit(token.front())) throw std::runtime_error("token must not start with a number!");

			insert_or_emplace(constantsAndVariables_, token, val);
		}

		template <class ... Ts>
		template <class T>
		void CompilerEntityDataBase<Ts...>::addVariable(const std::string& token, T* ptr)
		{
			if(token.empty()) throw std::runtime_error("token must not be empty!");
			if(!isAllAlnum(token)) throw std::runtime_error("token must be alphanumeric!");
			if(isdigit(token.front())) throw std::runtime_error("token must not start with a number!");

			insert_or_emplace(constantsAndVariables_, token, UntypedValue<Ts...>(ptr));
		}

		template <class ... Ts>
		template <class T>
		void CompilerEntityDataBase<Ts...>::addConstant(const std::string& token, T val)
		{
			if(token.empty()) throw std::runtime_error("token must not be empty!");
			if(!isAllAlnum(token)) throw std::runtime_error("token must be alphanumeric!");
			if(isdigit(token.front())) throw std::runtime_error("token must not start with a number!");

			insert_or_emplace(constantsAndVariables_, token, UntypedValue<Ts...>(val));

		}

		template <class... Ts>
		template <class T>
		constexpr TYPE CompilerEntityDataBase<Ts...>::type()
		{
			return classToType2<T, Ts...>();
		}
	}
}