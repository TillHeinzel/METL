#pragma once

#include <map>

#include "VarExpression.h"
#include "Stack.fwd.h"

namespace metl
{
	template<class... Ts>
	class Compiler
	{
	public:
		using Expression = Expression2<Ts...>;

		// the class assumes that Ts = {integer-type, float-type, other types...}. 
		// This is literally only necessary to make literals work (excuse the pun)
		static_assert(sizeof...(Ts) >= 2, "require at least two types (integer type and floating point type) for metl-compiler");
		using IntType = Get_t<0, Ts...>; // get 0th type from parameter pack
		using FloatType = Get_t<1, Ts...>;// get 1st type from parameter pack

		static_assert(std::is_constructible_v<IntType, int>, "IntType must be constructible from int! (integer literals are turned into int and then into IntType via static_cast)");
		static_assert(std::is_constructible_v<FloatType, double>, "FloatType must be constructible from double! (floating point literals are turned into double and then into FloatType via static_cast)");

	public:

		Compiler();

		Expression build(const std::string& expression);

		template<class T>
		exprType<T> build(const std::string& expression);

	public:
		void setOperatorPrecedence(std::string op, unsigned int precedence, ASSOCIATIVITY associativity = ASSOCIATIVITY::LEFT);
		void setUnaryOperatorPrecedence(std::string op, unsigned int precedence);

		void setCast(TYPE from, TYPE to, const CastImpl<Expression>& fs);

		void setOperator(const std::string& token, const std::vector<TYPE>& paramTypes, const FunctionImpl<Expression>& op);
		
		template<class Left, class Right, /*inferred*/ class F>
		void setOperator(std::string token, const F& f);

		void setUnaryOperator(const std::string& token, TYPE paramType, const FunctionImpl<Expression>& op);
		
		template<class T, /*inferred*/ class F>
		void setUnaryOperator(std::string token, const F& f);

		void setFunction(const std::string& token, const std::vector<TYPE>& paramTypes, const FunctionImpl<Expression>& function);
		
		template<class... ParamTypes, /*inferred*/ class F>
		void setFunction(const std::string& token, const F& f);

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

	public:


		const auto& getOperators() { return operators_; }
		const auto& getCandV() { return constantsAndVariables_; }
		const auto& getCarriers() { return opCarriers_; }
		const auto& getUnaryCarriers() { return unaryCarriers_; }
		const auto& getFunctionNames() { return functionNames_; }
		const auto& getFunctions() { return functions_; }
		const auto& getcastImplementations() { return castImplementations_; }
		const auto& getcastDeclarations() { return castDeclarations_; }


	public:
		Stack<Ts...> stack_;

	private:

		std::map<std::string, CastImpl<Expression>> castImplementations_;
		std::map<TYPE, std::vector<TYPE>> castDeclarations_;

		std::map<std::string, opCarrier> opCarriers_; // maps unmangled operators to their precedence
		std::map<std::string, opCarrier> unaryCarriers_; // maps unmangled operators to their precedence
		std::map<std::string, FunctionImpl<Expression>> operators_; //maps mangled names to implementations for operators
		std::map<std::string, FunctionImpl<Expression>> functions_; // maps mangled names to implementations for functions.
		std::map<std::string, std::string> functionNames_; // dummy, just so we have the sorting.
		std::map<std::string, Expression> constantsAndVariables_; // maps identifiers for constants and variables to the expressions returning their values.

		void addConstantOrVariable(std::string token, const Expression& val);

	};
}
