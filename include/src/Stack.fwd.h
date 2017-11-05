#pragma once
#include <vector>
#include <functional>
#include <string>

#include "VarExpression.h"

namespace metl
{
	template<class ExprT> using CastImpl = std::function<ExprT(ExprT)>;
	template<class ExprT> using FunctionImpl = std::function<ExprT(const std::vector<ExprT>&)>;

	template<class Expression, class T>
	Expression makeConstExpression(const T& v)
	{
		auto f = [v]()
		{
			return v;
		};

		return Expression(exprType<T>(f), CATEGORY::CONSTEXPR);
	}

	enum class ASSOCIATIVITY{LEFT,RIGHT};

	struct opCarrier
	{
		std::string name;
		unsigned int precedence;
		ASSOCIATIVITY associativity;
		bool isUnary;
	};
}

namespace metl
{
	template<class... Ts>
	class Stack
	{
		using Expression = Expression2<Ts...>;

		class Substack
		{
		public:

			explicit Substack(const std::map<std::string, FunctionImpl<Expression>>& opMap, 
				const std::map<std::string, FunctionImpl<Expression>>& functionMap,
				const std::map<std::string, CastImpl<Expression>>& castImplementations,
				const std::map<TYPE, std::vector<TYPE>>& castDeclarations);


			void push(const Expression l);
			void push(const opCarrier& b);
			void pushFunction(std::string FunctionName);

			Expression finish();

			void evaluateFunction();

			void reduce();
			void reduceBinary();
			void reduceUnary();

			bool empty() const { return expressions_.empty(); }
		private:
			bool plannedSignSwitch = false;

			std::vector< Expression > expressions_;
			std::vector< opCarrier > operators_;

			std::unique_ptr<std::string> function_;

			const std::map<std::string, FunctionImpl<Expression>>& opMap_;
			const std::map<std::string, FunctionImpl<Expression>>& functionMap_;

			const std::map<std::string, CastImpl<Expression>>& castImplementations_;
			const std::map<TYPE, std::vector<TYPE>>& castDeclarations_;


		private:
			//void tryCast()
			bool are_all_const(const std::vector<Expression>& expressions);
			void castTo(const std::vector<TYPE>& targetTypes);
		};

	public:
		Stack(const std::map<std::string, FunctionImpl<Expression>>& opMap, 
			const std::map<std::string, FunctionImpl<Expression>>& funcMap,
			const std::map<std::string, CastImpl<Expression>>& castImplementations,
			const std::map<TYPE, std::vector<TYPE>>& castDeclarations);

		void push(const Expression& t);

		void pushFunction(std::string functionName);

		void push(const opCarrier& op);

		void open();

		void close();

		Expression finish();
		void clear();

	private:
		std::vector<Substack> subStacks_;

		const std::map<std::string, FunctionImpl<Expression>>& opMap_;
		const std::map<std::string, FunctionImpl<Expression>>& funcMap_;

		const std::map<std::string, CastImpl<Expression>>& castImplementations_;
		const std::map<TYPE, std::vector<TYPE>>& castDeclarations_;
	};
}
