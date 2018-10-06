#pragma once

#include "src/TypeErasure/UntypedExpression.h"
#include "src/CompilerEntityDataBase.h"
#include "src/Caster.h"

namespace metl
{
	namespace internal
	{
		template<class... Ts>
		class ExpressionSubStack
		{
			using Expression = UntypedExpression<Ts...>;
		public:
			explicit ExpressionSubStack(const CompilerEntityDataBase<Ts...>& dataBase);

			void push(Expression l);
			void push(const opCarrier& b);
			void push(const suffixCarrier& suffix);

			Expression finish();

			bool empty() const;
		private:
			UntypedExpression<Ts...> applySuffix(const std::string& suffix, const Expression& argument) const;

			void reduce();
			void reduceUnary();
			UntypedExpression<Ts...> applyUnary(const std::string& opName, const Expression& arguments) const;

			void reduceBinary();

			UntypedExpression<Ts...> applyBinary(const std::string& opName, const std::vector<Expression>& arguments) const;

			std::vector<Expression> subExpressions_;
			std::vector<opCarrier> operators_;

			const CompilerEntityDataBase<Ts...>& dataBase_;
			const Caster<Ts...> caster_;
		};

	}
}
