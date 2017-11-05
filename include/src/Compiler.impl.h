#pragma once
#include "Compiler.fwd.h"

#include <tao/pegtl.hpp>

#include "grammar.h"
#include "actions.h"

#include "nameMangling.h"
#include "Stack.impl.h"

#include "CompilerHelpers.h"

namespace
{
	template<class T>
	void insert_or_emplace(std::map<std::string, T>& map, const std::string& key, const T& val)
	{
		auto it = map.find(key);
		if (it == map.end())
		{
			map.emplace(key, val);
			return;
		}
		it->second = val;
	}
}

namespace metl
{


	template <class ... Ts>
	Compiler<Ts...>::Compiler() :
		stack_(operators_, functions_, castImplementations_, castDeclarations_),
		castDeclarations_({ std::make_pair(type<Ts>(), std::vector<TYPE>{type<Ts>()})... })
	{
	}

	template <class ... Ts>
	typename Compiler<Ts...>::Expression Compiler<Ts...>::build(const std::string& expression)
	{
		stack_.clear();
		tao::pegtl::memory_input<> input(expression, std::string{});
		tao::pegtl::parse<grammar, action>(input, *this);

		return stack_.finish();
	}

	template <class ... Ts>
	template <class T>
	exprType<T> Compiler<Ts...>::build(const std::string& expression)
	{
		return build(expression).template get<T>();
	}

	template <class ... Ts>
	void Compiler<Ts...>::setOperatorPrecedence(std::string op, unsigned int precedence, ASSOCIATIVITY associativity)
	{
		// if operator already exists, replace precedence. Else, insert new operator with new precedence

		insert_or_emplace(opCarriers_, op, opCarrier{ op, precedence, associativity, false });
	}

	template <class ... Ts>
	void Compiler<Ts...>::setUnaryOperatorPrecedence(std::string op, unsigned precedence)
	{
		insert_or_emplace(unaryCarriers_, op, opCarrier{ op, precedence, ASSOCIATIVITY::RIGHT, true });
	}


	template <class ... Ts>
	void Compiler<Ts...>::setOperator(const std::string& token, const std::vector<TYPE>& paramTypes, const FunctionImpl<Expression>& op)
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
	template <class Left, class Right, class F>
	void Compiler<Ts...>::setOperator(std::string token, const F& f)
	{
		setOperator(token, {type<Left>(), type<Right>()}, metl::makeFunction<Expression, Left, Right>(f));
	}

	template <class ... Ts>
	void Compiler<Ts...>::setUnaryOperator(const std::string& token,TYPE paramType, const FunctionImpl<Expression>& op)
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
	template <class T, class F>
	void Compiler<Ts...>::setUnaryOperator(std::string token, const F& f)
	{
		setUnaryOperator(token, type<T>(), metl::makeFunction<Expression, T>(f));
	}

	template <class ... Ts>
	void Compiler<Ts...>::setFunction(const std::string& token, const std::vector<TYPE>& paramTypes, const FunctionImpl<Expression>& function)
	{
		insert_or_emplace(functionNames_, token, token);
		insert_or_emplace(functions_, mangleName(token, paramTypes), function);
	}

	template <class ... Ts>
	template <class ... ParamTypes, class F>
	void Compiler<Ts...>::setFunction(const std::string& token, const F& f)
	{
		setFunction(token, std::vector<TYPE>{type<ParamTypes>()...}, metl::makeFunction<Expression, ParamTypes...>(f));
	}

	template <class ... Ts>
	template <class From, class F>
	void Compiler<Ts...>::setCast(const F& f)
	{
		using To = std::result_of_t<F(From)>;
		static_assert(isInList<From, Ts...>(), "Type casted from is not one of the types of this metl");
		static_assert(isInList<To, Ts...>(), "Type casted to is not one of the types of this metl");

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

		setCast(type<From>(), type<To>(), CastImpl<Expression>(impl));
	}

	template <class ... Ts>
	template <class T>
	void Compiler<Ts...>::setConstant(const std::string& token, T&& val)
	{
		/*
					static_assert(isInList<T, Ts...>(), "T must be one of the types the compiler works with.");*/
		addConstantOrVariable(token, makeConstExpression<Expression>(std::forward<T>(val)));
	}

	template <class ... Ts>
	template <class T>
	void Compiler<Ts...>::setVariable(const std::string& token, T* val)
	{
		addConstantOrVariable(token, makeVariableExpression<Expression>(val));
	}

	template <class ... Ts>
	void Compiler<Ts...>::setCast(TYPE from, TYPE to, const CastImpl<Expression>& fs)
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
	void Compiler<Ts...>::addConstantOrVariable(std::string token, const Expression& val)
	{
		if (token.empty()) throw std::runtime_error("token must not be empty!");
		if (isdigit(token.front())) throw std::runtime_error("token must not start with a number!");

		insert_or_emplace(constantsAndVariables_, token, val);
	}

	template <class ... Ts>
	template <class T>
	constexpr TYPE Compiler<Ts...>::type()
	{
		return classToType2<T, Ts...>();
	}
}
