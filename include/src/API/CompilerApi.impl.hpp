// File: CompilerApi.impl.h
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
#include "CompilerApi.hpp"

#include "src/Compiler.impl.hpp"

#include "src/VariableExpression.hpp"

namespace metl
{
	template <class Grammar, class LiteralsConverters, class ... Ts>
	CompilerApi<Grammar, LiteralsConverters, Ts...>::CompilerApi(const LiteralsConverters& literalConverters) :
		impl_(literalConverters)
	{

	}

	namespace detail
	{
		template<class Expr>
		void castToAll(Expr&, const std::map<std::string, internal::UntypedConversion<Expr>>&)
		{}

		template<class T, class... Ts, class Expr>
		void castToAll(Expr& expr, const std::map<std::string, internal::UntypedConversion<Expr>>& castImpls)
		{
			auto it = castImpls.find(internal::mangleCast(expr.type(), expr.template toType<T>()));
			if(it != castImpls.end())
			{
				expr.template set<T>(it->second(expr).template get<T>());
			}
			castToAll<Ts...>(expr, castImpls);
		}
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	typename CompilerApi<Grammar, LiteralConverters, Ts...>::OutExpression CompilerApi<Grammar, LiteralConverters, Ts...>::build(const std::string& expression)
	{
		impl_.stack_.clear();
		tao::pegtl::memory_input<> input(expression, std::string{});
		tao::pegtl::parse<Grammar, internal::action>(input, impl_);

		auto expr = impl_.finish();

		return {expr, impl_.bits_.getAllCastsFrom(expr.type())};
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T>
	TypedExpression<T> CompilerApi<Grammar, LiteralConverters, Ts...>::build(const std::string& expression)
	{
		return build(expression).template get<T>();
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	void CompilerApi<Grammar, LiteralConverters, Ts...>::setOperatorPrecedence(const std::string op, const unsigned int precedence, const ASSOCIATIVITY associativity)
	{
		impl_.bits_.setOperatorPrecedence(op, precedence, associativity);
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	void CompilerApi<Grammar, LiteralConverters, Ts...>::setUnaryOperatorPrecedence(const std::string op, const unsigned precedence)
	{
		impl_.bits_.setUnaryOperatorPrecedence(op, precedence);
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class Left, class Right, class F>
	void CompilerApi<Grammar, LiteralConverters, Ts...>::setOperator(const std::string& token, const F& f)
	{
		impl_.bits_.setOperator(token, {type<Left>(), type<Right>()}, metl::internal::makeUntypedFunction<Expression, Left, Right>(f));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T, class F>
	void CompilerApi<Grammar, LiteralConverters, Ts...>::setUnaryOperator(const std::string& token, const F& f)
	{
		impl_.bits_.setUnaryOperator(token, type<T>(), metl::internal::makeUntypedFunction<Expression, T>(f));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class ... ParamTypes, class F>
	void CompilerApi<Grammar, LiteralConverters, Ts...>::setFunction(const std::string& token, const F& f)
	{
		impl_.bits_.setFunction(token, std::vector<TYPE>{type<ParamTypes>()...}, metl::internal::makeUntypedFunction<Expression, ParamTypes...>(f));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class From, class F>
	void CompilerApi<Grammar, LiteralConverters, Ts...>::setCast(const F& f)
	{
		using To = std::result_of_t<F(From)>;

		static_assert(internal::isInList<From, Ts...>(), "Type casted from is not one of the types of this compiler!");
		static_assert(internal::isInList<To, Ts...>(), "Type casted to is not one of the types of this compiler!");

		impl_.bits_.setCast(type<From>(), type<To>(), internal::makeDynamicConversion<Expression, From>(f));
	}

	template <class Grammar, class LiteralsConverters, class ... Ts>
	template <class From, class To, class F>
	void CompilerApi<Grammar, LiteralsConverters, Ts...>::setSuffix(const std::string& token, const F& f)
	{
		using namespace internal;

		static_assert(isInList<From, Ts...>(), "Type the suffix converts from is not one of the types of this compiler!");
		static_assert(isInList<To, Ts...>(), "Type the suffix converts to is not one of the types of this compiler!");

		impl_.bits_.setSuffix(token, type<From>(), internal::makeDynamicConversion<Expression, From>(f));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T>
	void CompilerApi<Grammar, LiteralConverters, Ts...>::setConstant(const std::string& token, T&& val)
	{
		impl_.bits_.addConstant(token, val);
	}

	template <class Grammar, class LiteralsConverters, class ... Ts>
	template <class T>
	T CompilerApi<Grammar, LiteralsConverters, Ts...>::getValue(const std::string& token)
	{
		return getValue(token).template get<T>()();
	}

	template <class Grammar, class LiteralsConverters, class ... Ts>
	typename CompilerApi<Grammar, LiteralsConverters, Ts...>::Expression CompilerApi<Grammar, LiteralsConverters, Ts...>::
		getValue(const std::string& token)
	{
		auto valueOpt = impl_.bits_.findValue(token);

		if(valueOpt)
		{
			return valueOpt->makeUntypedExpression();
		}
		throw std::runtime_error("Variable with name \"" + token + "\" could not be found");
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T>
	void CompilerApi<Grammar, LiteralConverters, Ts...>::setVariable(const std::string& token, T* val)
	{
		impl_.bits_.addVariable(token, val);
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T>
	constexpr TYPE CompilerApi<Grammar, LiteralConverters, Ts...>::type()
	{
		return classToType2<T, Ts...>();
	}
}
