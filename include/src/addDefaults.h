/*
@file
addDefaults.h
Helper-functions to add default operators, functions, etc. to a compiler

Copyright 2017 Till Heinzel

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
#include "Compiler.h"

namespace metl
{
	template<class... Ts>
	void setDefaultOperatorPrecedences(Compiler<Ts...>& c)
	{
		c.setOperatorPrecedence("+", 3);
		c.setOperatorPrecedence("-", 3);
		c.setOperatorPrecedence("*", 2);
		c.setOperatorPrecedence("/", 2);
		c.setUnaryOperatorPrecedence("-", 1);
		c.setUnaryOperatorPrecedence("+", 1);
		c.setOperatorPrecedence("^", 0);
	}

	// sets default unary operator {+,-} and binary operator {+,-,*,/} with the corresponding call in C++ 
	template<class T, class... Ts>
	void addDefaultOperators(Compiler<Ts...>& c, const T&)
	{
		setDefaultOperatorPrecedences(c);

		auto unaryPlus = [](auto a) { return a; };
		auto unaryMinus = [](auto a) { return -a; };

		c.template setUnaryOperator<T>(std::string("+"), unaryPlus);
		c.template setUnaryOperator<T>("-", unaryMinus);

		auto plus = [](auto left, auto right) { return left + right; };
		auto minus = [](auto left, auto right) { return left - right; };
		auto mul = [](auto left, auto right) { return left * right; };
		auto div = [](auto left, auto right) { return left / right; };

		c.template setOperator<T, T>(std::string("+"), plus);
		c.template setOperator<T, T>("-", minus);
		c.template setOperator<T, T>("*", mul);
		c.template setOperator<T, T>("/", div);
	}

	// sets default binary operators {+,-,*,/} with the corresponding call in C++ 
	template<class T1, class T2, class... Ts>
	void addDefaultOperators(Compiler<Ts...>& c, const T1&, const T2&)
	{
		setDefaultOperatorPrecedences(c);

		auto plus = [](auto left, auto right) { return left + right; };
		auto minus = [](auto left, auto right) { return left - right; };
		auto mul = [](auto left, auto right) { return left * right; };
		auto div = [](auto left, auto right) { return left / right; };

		c.template setOperator<T1, T2>(std::string("+"), plus);
		c.template setOperator<T1, T2>("-", minus);
		c.template setOperator<T1, T2>("*", mul);
		c.template setOperator<T1, T2>("/", div);
	}

	template<class T, class... Ts>
	void addBasicFunctions(Compiler<Ts...>& c, const T&)
	{
		c.template setFunction<T>("exp", [](auto a) {return exp(a); });
		c.template setFunction<T>("abs", [](auto a) {return abs(a); });
		c.template setFunction<T>("sqrt", [](auto a) {return sqrt(a); });
		c.template setFunction<T>("exp2", [](auto a) {return exp2(a); });
		c.template setFunction<T>("log", [](auto a) {return log(a); });
		c.template setFunction<T>("log2", [](auto a) {return log2(a); });
		c.template setFunction<T>("log10", [](auto a) {return log10(a); });

	}

	template<class T, class... Ts>
	void addTrigFunctions(Compiler<Ts...>& c, const T&)
	{
		c.template setFunction<T>("sin", [](auto a) {return sin(a); });
		c.template setFunction<T>("cos", [](auto a) {return cos(a); });
		c.template setFunction<T>("tan", [](auto a) {return tan(a); });
		c.template setFunction<T>("asin", [](auto a) {return asin(a); });
		c.template setFunction<T>("acos", [](auto a) {return acos(a); });
		c.template setFunction<T>("atan", [](auto a) {return atan(a); });
		c.template setFunction<T>("sinh", [](auto a) {return sinh(a); });
		c.template setFunction<T>("cosh", [](auto a) {return cosh(a); });
		c.template setFunction<T>("tanh", [](auto a) {return tanh(a); });
		c.template setFunction<T>("asinh", [](auto a) {return asinh(a); });
		c.template setFunction<T>("acosh", [](auto a) {return acosh(a); });
		c.template setFunction<T>("atanh", [](auto a) {return atanh(a); });
	}

	// sets defaults for basic types (currently int, 
	template<class Grammar, class Converter, class... Ts>
	void setDefaults(Compiler<Grammar, Converter, Ts...>& c)
	{
		using intType = decltype(c.impl_.literalConverters_.toInt(std::declval<std::string>()));
		using realType = decltype(c.impl_.literalConverters_.toReal(std::declval<std::string>()));
		
		// defaults with intType
		constexpr_if(IsInList<intType, Ts...>(), [&c](auto _) 
		{
			addDefaultOperators(c, _(intType()));
		});

		// defaults with realType
		constexpr_if(IsInList<realType, Ts...>(), [&c](auto _) 
		{
			addDefaultOperators(c, _(realType()));
			addBasicFunctions(c, _(realType()));
			addTrigFunctions(c, _(realType()));
		});		

		// if both integer and real-types exist, add cast from int to real
		constexpr_if(std::integral_constant<bool, isInList<intType, Ts...>() && isInList<realType, Ts...>()>{},
			[&c](auto _)
		{
			c.template setCast<intType>(_([](const intType& in)
			{
				return static_cast<realType>(in);
			}));
		}
		);
	}
}


/*
#include "QExpressionBuilder.h"

namespace expressionbuilder
{
	template<class T, class Builder, class F>
	void add_binaryOperators(Builder&, std::string, const F&)
	{
	}

	// adds all combinations of binary operators having T as the left side and T2, Ts... as the right side.
	// so add_binaryOperators<T, T2, T3, T4>(b, "+", [](auto a, auto b){return a+b;}) creates plus-operators for T+T2, T+T3 and T+T4
	template<class T, class T2, class... Ts, class Builder, class F>
	void add_binaryOperators(Builder& builder, std::string token, const F& f)
	{
		builder.template setOperator<T, T2>(token, f);
		add_binaryOperators<T, Ts...>(builder, token, f);
	}


	template<class Builder, class F>
	void add_unaryOperators(Builder&, std::string, const F&)
	{
	}

	template<class T, class... Ts, class Builder, class F>
	void add_unaryOperators(Builder& builder, std::string token, const F& f)
	{
		builder.template setUnaryOperator<T>(token, f);
		add_unaryOperators<Ts...>(builder, token, f);
	}
}

namespace expressionbuilder
{
	void addDefaults(QExpressionBuilder& builder)
	{
		using namespace qengine;
		using Expression = QExpressionBuilder::Expression;

		builder.setCast<int>([](int a) { return static_cast<double>(a); });

		//// Default constants
		builder.setConstant("pi", PI);
		builder.setConstant("i", IMAGUNIT);
		builder.setConstant("e", std::exp(1));

		////operator precedence
		builder.setOperatorPrecedence("+", 3);
		builder.setOperatorPrecedence("-", 3);
		builder.setOperatorPrecedence("*", 2);
		builder.setOperatorPrecedence("/", 2);
		builder.setUnaryOperatorPrecedence("-", 1);
		builder.setUnaryOperatorPrecedence("+", 1);
		builder.setOperatorPrecedence("^", 0);

		//// unary operators
		auto fUnaryPlus = [](auto a) { return a; };
		auto fUnaryMinus = [](auto a) { return -a; };

		add_unaryOperators<int, real, complex, RVec, CVec>(builder, "+", fUnaryPlus);
		add_unaryOperators<int, real, complex, RVec, CVec>(builder, "-", fUnaryMinus);

		//// Arithmetic
		// add arithmetic operators for each combination of input parameters. add_next<Operator, T, Ts...>(v) adds the Operator for all combinations of T on the left and Ts... on the right.

		auto fplus = [](auto left, auto right) { return left + right; };
		auto fminus = [](auto left, auto right) { return left - right; };
		auto fmul = [](auto left, auto right) { return left * right; };
		auto fpwmul = [](auto left, auto right) { return left % right; };
		auto fdiv = [](auto left, auto right) { return left / right; };

		add_binaryOperators<int, int>(builder, "+", fplus);
		add_binaryOperators<real, real, complex, RVec, CVec>(builder, "+", fplus);
		add_binaryOperators<complex, real, complex, RVec, CVec>(builder, "+", fplus);
		add_binaryOperators<RVec, real, complex, RVec, CVec>(builder, "+", fplus);
		add_binaryOperators<CVec, real, complex, RVec, CVec>(builder, "+", fplus);

		add_binaryOperators<int, int>(builder, "-", fminus);
		add_binaryOperators<real, real, complex, RVec, CVec>(builder, "-", fminus);
		add_binaryOperators<complex, real, complex, RVec, CVec>(builder, "-", fminus);
		add_binaryOperators<RVec, real, complex, RVec, CVec>(builder, "-", fminus);
		add_binaryOperators<CVec, real, complex, RVec, CVec>(builder, "-", fminus);

		add_binaryOperators<int, int>(builder, "*", fmul);
		add_binaryOperators<real, real, complex, RVec, CVec>(builder, "*", fmul);
		add_binaryOperators<complex, real, complex, RVec, CVec>(builder, "*", fmul);
		add_binaryOperators<RVec, real, complex>(builder, "*", fmul);
		add_binaryOperators<CVec, real, complex>(builder, "*", fmul);

		add_binaryOperators<RVec, RVec, CVec>(builder, "*", fpwmul);
		add_binaryOperators<CVec, RVec, CVec>(builder, "*", fpwmul);

		add_binaryOperators<real, real, complex, RVec, CVec>(builder, "/", fdiv);
		add_binaryOperators<complex, real, complex, RVec, CVec>(builder, "/", fdiv);
		add_binaryOperators<RVec, real, complex, RVec, CVec>(builder, "/", fdiv);
		add_binaryOperators<CVec, real, complex, RVec, CVec>(builder, "/", fdiv);

		auto fpowScalar = [](const auto& a, const auto& b) { return pow(a, b); };
		auto fpowVector = [](auto v, const auto& a)
		{
			for (auto i = 0u; i<v.size(); ++i)
			{
				v.at(i) = std::pow(v.at(i), a);
			}

			return v;
		};
		builder.setOperator<real, real>("^", fpowScalar);
		builder.setOperator<complex, real>("^", fpowScalar);
		builder.setOperator<RVec, real>("^", fpowVector);
		builder.setOperator<CVec, real>("^", fpowVector);

		// default functions
		// setFunction takes input parameters as template parameters, then name, and finally  a lambda containing the implementation.

		builder.setFunction<real, real>("pow", fpowScalar);
		builder.setFunction<complex, real>("pow", fpowScalar);
		builder.setFunction<RVec, real>("pow", fpowVector);
		builder.setFunction<CVec, real>("pow", fpowVector);

		auto gauss = [](const RVec& x, real front, real width, real center)
		{
			return front * exp(-1 / (2 * width * width) * pow(x - center, 2));
		};
		builder.setFunction<RVec, real, real, real>("gauss", gauss);

		// single-valued functions for all types easily
		auto addSingleFunc = [&builder](const std::string& name, auto F)
		{
			builder.setFunction<real>(name, F);
			builder.setFunction<complex>(name, F);
			builder.setFunction<RVec>(name, F);
			builder.setFunction<CVec>(name, F);
		};

		// misc
		addSingleFunc("exp", [](const auto& a) { return exp(a); });
		addSingleFunc("sqrt", [](const auto& a) { return sqrt(a); });

		builder.setFunction<real>("abs", [](const auto& a) { return std::abs(a); });
		builder.setFunction<complex>("abs", [](const auto& a) { return std::abs(a); });
		builder.setFunction<RVec>("abs", [](const auto& a) { return abs(a); });
		builder.setFunction<CVec>("abs", [](const auto& a) { return abs(a); });

		builder.setFunction<real>("exp2", [](const auto& a) { return exp2(a); });
		builder.setFunction<RVec>("exp2", [](const auto& a) { return exp2(a); });

		builder.setFunction<real>("log", [](const auto& a) { return log(a); });
		builder.setFunction<RVec>("log", [](const auto& a) { return log(a); });

		builder.setFunction<real>("log2", [](const auto& a) { return log2(a); });
		builder.setFunction<RVec>("log2", [](const auto& a) { return log2(a); });

		builder.setFunction<real>("log10", [](const auto& a) { return log10(a); });
		builder.setFunction<RVec>("log10", [](const auto& a) { return log10(a); });

		builder.setFunction<complex>("re", [](const auto& a) { return std::real(a); });
		builder.setFunction<CVec>("re", [](const auto& a) { return re(a); });
		builder.setFunction<complex>("im", [](const auto& a) { return std::imag(a); });
		builder.setFunction<CVec>("im", [](const auto& a) { return im(a); });

		// trig
		addSingleFunc("sin", [](const auto& a) { return sin(a); });
		addSingleFunc("cos", [](const auto& a) { return cos(a); });
		addSingleFunc("tan", [](const auto& a) { return tan(a); });
		addSingleFunc("asin", [](const auto& a) { return asin(a); });
		addSingleFunc("acos", [](const auto& a) { return acos(a); });
		addSingleFunc("atan", [](const auto& a) { return atan(a); });
		addSingleFunc("sinh", [](const auto& a) { return sinh(a); });
		addSingleFunc("cosh", [](const auto& a) { return cosh(a); });
		addSingleFunc("tanh", [](const auto& a) { return tanh(a); });
		addSingleFunc("asinh", [](const auto& a) { return asinh(a); });
		addSingleFunc("acosh", [](const auto& a) { return acosh(a); });
		addSingleFunc("atanh", [](const auto& a) { return atanh(a); });
	}
}
*/
