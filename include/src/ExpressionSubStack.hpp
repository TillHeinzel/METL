#pragma once

#include "src/TypeErasure/UntypedExpression.h"
#include "src/CompilerEntityDataBase.h"

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
			void push(const BasicOperatorData& b);
			void push(const SuffixID& suffix);

			Expression finish();

			bool empty() const;
		private:
			void reduce();
			void reduceUnary();
			void reduceBinary();

			std::vector<Expression> subExpressions_;
			std::vector<BasicOperatorData> operators_;

			const OperationApplier<Ts...> operationApplier_;
		};

	}
}
