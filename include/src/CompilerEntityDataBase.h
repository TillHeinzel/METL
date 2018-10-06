// File: CompilerEntityDataBase.h
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

#include <cassert>

#include "ThirdParty/Optional/optional.hpp"

#include "src/TypeErasure/UntypedExpression.h"
#include "src/TypeErasure/UntypedFunction.h"
#include "src/TypeErasure/UntypedConversion.h"
#include "src/TypeErasure/UntypedValue.h"

#include "src/SuffixID.h"
#include "src/BasicOperatorData.h"

#include "Associativity.h"
#include "nameMangling.h"

namespace metl
{
	namespace internal
	{
		template<typename Input, class T>
		static auto match_any_recursive(Input& in, const std::map<std::string, T>& map, std::string t)
			-> typename std::map<std::string, T>::const_iterator
		{
			// t starts out empty, and slowly becomes the same as in.current, adding one char on each recursion-step. t is then searched in the keys of map.
			if(in.size() > t.size())
			{
				t += in.peek_char(t.size()); // append another char to teststring t
				const auto i = map.lower_bound(t); // not sure why this works
				if(i != map.end())
				{
					// recursion step. try the next one down the line with an additional char.
					// This happend before the termination, because we are expected to be greedy.
					auto i2 = match_any_recursive(in, map, t);
					if(i2 != map.end())
					{
						return i2;
					}
					if(i->first == t)  // recursion stop: if we found a match, return.
					{
						return i;
					}
				}
			}
			return map.end();
		}

		template<class... Ts>
		class CompilerEntityDataBase
		{

		public:
			using Expression = UntypedExpression<Ts...>;
			constexpr static auto getTypeList()
			{
				return TypeList<Ts...>();
			}

			void setOperatorPrecedence(const std::string& op, unsigned int precedence, ASSOCIATIVITY associativity = ASSOCIATIVITY::LEFT);
			void setUnaryOperatorPrecedence(const std::string& op, unsigned int precedence);

			void setOperator(const std::string& token, const std::vector<TYPE>& paramTypes, const UntypedFunction<Expression>& op);

			void setUnaryOperator(const std::string& token, TYPE paramType, const UntypedFunction<Expression>& op);

			void setFunction(const std::string& token, const std::vector<TYPE>& paramTypes, const UntypedFunction<Expression>& function);

			void setCast(TYPE from, TYPE to, const UntypedConversion<Expression>& fs);

			void setSuffix(const std::string& token, TYPE from, const UntypedConversion<Expression>& conversion);

			void addConstantOrVariable(const std::string& token, const UntypedValue<Ts...>& val);

			template<class T>
			void addVariable(const std::string& token, T* ptr);

			template<class T>
			void addConstant(const std::string& token, T val);

			template<class T>
			constexpr static TYPE type();

			template<class Input>
			auto matchVariable(Input& in)
			{
				return match(in, constantsAndVariables_);
			}

			template<class Input>
			auto matchOperator(Input& in)
			{
				return match(in, opCarriers_);
			}

			template<class Input>
			auto matchUnaryOperator(Input& in)
			{
				return match(in, unaryCarriers_);
			}

			template<class Input>
			auto matchSuffix(Input& in)
			{
				return match(in, suffixes_);
			}

			template<class Input>
			auto matchFunctionName(Input& in)
			{
				return match(in, functionNames_);
			}

			tl::optional<UntypedFunction<Expression>> findFunction(const std::string& mangledName) const
			{
				return find(functions_, mangledName);
			}
			
			tl::optional<UntypedFunction<Expression>> findOperator(const std::string& mangledName) const
			{
				return find(operators_, mangledName);
			}
			
			tl::optional<UntypedConversion<Expression>> findSuffix(const std::string& mangledName) const
			{
				return find(suffixImplementations_, mangledName);
			}

			tl::optional<UntypedValue<Ts...>> findValue(const std::string& name) const
			{
				return find(constantsAndVariables_, name);
			}

			tl::optional<UntypedConversion<Expression>> findCast(const std::string& mangledName) const
			{
				return find(castImplementations_, mangledName);
			}

			std::vector<UntypedConversion<Expression>> getAllCastsFrom(const TYPE& from) const
			{
				auto accessibleTypes = castDeclarations_.at(from);
				auto retval = std::vector<UntypedConversion<Expression>>{};

				for(const auto& to: accessibleTypes)
				{
					if(to != from)
					{
						retval.push_back(castImplementations_.at(mangleCast(from, to)));
					}
				}
				return retval;
			}

			std::vector<TYPE> getAllTypesCastableFrom(const TYPE& type) const
			{
				assert(!castDeclarations_.at(type).empty());
				return castDeclarations_.at(type);
			}

			


		private:
			std::map<std::string, UntypedConversion<Expression>> castImplementations_;
			std::map<std::string, UntypedValue<Ts...>> constantsAndVariables_;

			template<class Input, class Map>
			auto match(Input& in, const Map& map)
			{
				auto it = match_any_recursive(in, map, std::string());

				bool varMatched = (it == map.end()) ? false : true;

				return std::make_tuple(varMatched, it);
			}

			template<class Map, class Key>
			static tl::optional<typename Map::mapped_type> find(const Map& map, const Key& key)
			{
				auto it = map.find(key);
				if(it != map.end())
				{
					return {it->second};
				}
				return {};
			}

			std::map<std::string, UntypedConversion<Expression>> suffixImplementations_;
			std::map<std::string, SuffixID> suffixes_;

			std::map<TYPE, std::vector<TYPE>> castDeclarations_{std::make_pair(type<Ts>(), std::vector<TYPE>{type<Ts>()})...};

			std::map<std::string, BasicOperatorData> opCarriers_; // maps unmangled operators to their precedence
			std::map<std::string, BasicOperatorData> unaryCarriers_; // maps unmangled operators to their precedence

			std::map<std::string, UntypedFunction<Expression>> operators_; //maps mangled names to implementations for operators
			std::map<std::string, UntypedFunction<Expression>> functions_; // maps mangled names to implementations for functions.
			std::map<std::string, std::string> functionNames_; // dummy, just so we have the sorting.

		};
	}
}
