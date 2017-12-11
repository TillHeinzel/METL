#pragma once
#include "Compiler.fwd.h"

#include "Compiler_Detail.impl.h"


namespace metl
{

	//template <class Grammar, class LiteralConverters, class... Ts>
	//Compiler<Grammar, LiteralConverters, Ts...>::Compiler() :
	//	stack_(operators_, functions_, castImplementations_, castDeclarations_),
	//	castDeclarations_({ std::make_pair(type<Ts>(), std::vector<TYPE>{type<Ts>()})... })
	//{
	//}

	template <class Grammar, class LiteralsConverters, class ... Ts>
	Compiler<Grammar, LiteralsConverters, Ts...>::Compiler(const LiteralsConverters& literalConverters) :
	impl_(literalConverters)
	{

	}

	template <class Grammar, class LiteralConverters, class... Ts>
	typename Compiler<Grammar, LiteralConverters, Ts...>::Expression Compiler<Grammar, LiteralConverters, Ts...>::build(const std::string& expression)
	{
		impl_.stack_.clear();
		tao::pegtl::memory_input<> input(expression, std::string{});
		tao::pegtl::parse<Grammar, action>(input, *this);

		return impl_.stack_.finish();
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T>
	exprType<T> Compiler<Grammar, LiteralConverters, Ts...>::build(const std::string& expression)
	{
		return build(expression).template get<T>();
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	void Compiler<Grammar, LiteralConverters, Ts...>::setOperatorPrecedence(const std::string op, const unsigned int precedence, const ASSOCIATIVITY associativity)
	{
		impl_.setOperatorPrecedence(op, precedence, associativity);
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	void Compiler<Grammar, LiteralConverters, Ts...>::setUnaryOperatorPrecedence(const std::string op, const unsigned precedence)
	{
		impl_.setUnaryOperatorPrecedence(op, precedence);
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class Left, class Right, class F>
	void Compiler<Grammar, LiteralConverters, Ts...>::setOperator(const std::string token, const F& f)
	{
		impl_.setOperator(token, { type<Left>(), type<Right>() }, metl::makeFunction<Expression, Left, Right>(f));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T, class F>
	void Compiler<Grammar, LiteralConverters, Ts...>::setUnaryOperator(const std::string token, const F& f)
	{
		impl_.setUnaryOperator(token, type<T>(), metl::makeFunction<Expression, T>(f));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class ... ParamTypes, class F>
	void Compiler<Grammar, LiteralConverters, Ts...>::setFunction(const std::string& token, const F& f)
	{
		impl_.setFunction(token, std::vector<TYPE>{type<ParamTypes>()...}, metl::makeFunction<Expression, ParamTypes...>(f));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class From, class F>
	void Compiler<Grammar, LiteralConverters, Ts...>::setCast(const F& f)
	{
		using To = std::result_of_t<F(From)>;
		static_assert(isInList<From, Ts...>(), "Type casted from is not one of the types of this compiler");
		static_assert(isInList<To, Ts...>(), "Type casted to is not one of the types of this compiler");

		auto impl = [f](const Expression& from)
		{
			auto f_from = from.template get<From>();
			return Expression(exprType<To>{
				[f, f_from]()
				{
					return f(f_from());
				}
			});
		};

		impl_.setCast(type<From>(), type<To>(), CastImpl<Expression>(impl));
	}

	template <class Grammar, class LiteralsConverters, class ... Ts>
	template <class From, class To, class F>
	void Compiler<Grammar, LiteralsConverters, Ts...>::setSuffix(const std::string& token, const F& f)
	{
		static_assert(isInList<From, Ts...>(), "Type the suffix converts from is not one of the types of this compiler!");
		static_assert(isInList<To, Ts...>(), "Type the suffix converts to is not one of the types of this compiler!");

		auto impl = [f](const Expression& from)
		{
			auto f_from = from.template get<From>();
			return Expression(exprType<To>{
				[f, f_from]()
				{
					return f(f_from());
				}
			});
		};

		impl_.setSuffix(token, type<From>(), CastImpl<Expression>(impl));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T>
	void Compiler<Grammar, LiteralConverters, Ts...>::setConstant(const std::string& token, T&& val)
	{
		/*
					static_assert(isInList<T, Ts...>(), "T must be one of the types the compiler works with.");*/
		impl_.addConstantOrVariable(token, makeConstExpression<Expression>(std::forward<T>(val)));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T>
	void Compiler<Grammar, LiteralConverters, Ts...>::setVariable(const std::string& token, T* val)
	{
		impl_.addConstantOrVariable(token, makeVariableExpression<Expression>(val));
	}

	template <class Grammar, class LiteralConverters, class... Ts>
	template <class T>
	constexpr TYPE Compiler<Grammar, LiteralConverters, Ts...>::type()
	{
		return classToType2<T, Ts...>();
	}
}
