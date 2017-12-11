#pragma once

#include <map>

#include "Compiler_Detail.fwd.h"

namespace metl
{
	template<class Grammar, class LiteralsConverters, class... Ts>
	class Compiler
	{
	public:
		using Expression = Expression2<Ts...>;

	public:

		Compiler(const LiteralsConverters& literalConverters);

		Expression build(const std::string& expression);

		template<class T>
		exprType<T> build(const std::string& expression);

	public:
		void setOperatorPrecedence(std::string op, unsigned int precedence, ASSOCIATIVITY associativity = ASSOCIATIVITY::LEFT);
		void setUnaryOperatorPrecedence(std::string op, unsigned int precedence);

		template<class Left, class Right, /*inferred*/ class F>
		void setOperator(std::string token, const F& f);

		template<class T, /*inferred*/ class F>
		void setUnaryOperator(std::string token, const F& f);

		// Finds strings of the form token(ParamTypes...) and calls f on the params
		template<class... ParamTypes, /*inferred*/ class F>
		void setFunction(const std::string& token, const F& f);

		// tries to implicitly cast from type "From" to whatever type is returned by function f
		template<class From, /*inferred*/ class F>
		void setCast(const F& f);

	public:
		template<class T>
		void setConstant(const std::string& token, T&& val);

		template<class T>
		void setVariable(const std::string& token, T* val);
	public:
		template<class T>
		constexpr static TYPE type();

		detail::Compiler_impl<LiteralsConverters, Ts...> impl_;
	};
}
