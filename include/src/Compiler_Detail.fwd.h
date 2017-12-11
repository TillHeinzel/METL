#pragma once

#include "VarExpression.h"
#include "Stack.fwd.h"

namespace metl
{
	namespace detail
	{

		template<class LiteralConverters, class... Ts>
		class Compiler_impl
		{

			using Expression = Expression2<Ts...>;
		public:
			Compiler_impl(const LiteralConverters& literalConverters);


			void setOperatorPrecedence(std::string op, unsigned int precedence, ASSOCIATIVITY associativity = ASSOCIATIVITY::LEFT);
			void setUnaryOperatorPrecedence(std::string op, unsigned int precedence);

			void setOperator(const std::string& token, const std::vector<TYPE>& paramTypes, const FunctionImpl<Expression>& op);

			void setUnaryOperator(const std::string& token, TYPE paramType, const FunctionImpl<Expression>& op);

			void setFunction(const std::string& token, const std::vector<TYPE>& paramTypes, const FunctionImpl<Expression>& function);

			void setCast(TYPE from, TYPE to, const CastImpl<Expression>& fs);

			void setSuffix(const std::string& token, TYPE from, const CastImpl<Expression>& conversion);

			const auto& getOperators() { return operators_; }
			const auto& getCandV() { return constantsAndVariables_; }
			const auto& getCarriers() { return opCarriers_; }
			const auto& getUnaryCarriers() { return unaryCarriers_; }
			const auto& getFunctionNames() { return functionNames_; }
			const auto& getFunctions() { return functions_; }
			const auto& getcastImplementations() { return castImplementations_; }
			const auto& getcastDeclarations() { return castDeclarations_; }
			const auto& getSuffixes() { return suffixes_; }
			const auto& getSuffixImplementations() { return suffixes_; }


		public:
			Stack<Ts...> stack_;
			LiteralConverters literalConverters_;

		private:

			std::map<std::string, CastImpl<Expression>> castImplementations_;
			std::map<std::string, CastImpl<Expression>> suffixImplementations_;
			std::map<std::string, suffixCarrier> suffixes_;
			std::map<TYPE, std::vector<TYPE>> castDeclarations_;

			std::map<std::string, opCarrier> opCarriers_; // maps unmangled operators to their precedence
			std::map<std::string, opCarrier> unaryCarriers_; // maps unmangled operators to their precedence
			std::map<std::string, FunctionImpl<Expression>> operators_; //maps mangled names to implementations for operators
			std::map<std::string, FunctionImpl<Expression>> functions_; // maps mangled names to implementations for functions.
			std::map<std::string, std::string> functionNames_; // dummy, just so we have the sorting.
			std::map<std::string, Expression> constantsAndVariables_; // maps identifiers for constants and variables to the expressions returning their values.

			void addConstantOrVariable(std::string token, const Expression& val);

			template<class T>
			constexpr static TYPE type();
		};
	}
}
