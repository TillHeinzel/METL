#pragma once

#include "src/TypeErasure/UntypedExpression.hpp"

#include "src/CompilerEntityDataBase.hpp"
#include "src/OperationApplier.hpp"

namespace metl
{
	namespace internal
	{
		template<class... Ts>
		class FunctionSubStack
		{
			using Expression = UntypedExpression<Ts...>;
		public:
			FunctionSubStack(const CompilerEntityDataBase<Ts...>& dataBase, std::string functionName);

			void addArgument(Expression l);

			Expression finish();

			bool empty() const;

		private:
			const OperationApplier<Ts...> operationApplier_;

			const std::string functionName_;
			std::vector<Expression> arguments_;
		};
	}
}