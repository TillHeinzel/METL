#pragma once

#include "src/TypeErasure/UntypedExpression.h"

#include "src/CompilerEntityDataBase.h"
#include "src/Caster.h"

namespace metl
{
	namespace internal
	{
		template<class... Ts>
		class FunctionSubStack
		{
			using Expression = UntypedExpression<Ts...>;
		public:
			FunctionSubStack(const CompilerEntityDataBase<Ts...>& database, std::string functionName);

			void push(Expression l);

			Expression finish();

			bool empty() const;

		private:
			void evaluateFunction();

			const CompilerEntityDataBase<Ts...>& dataBase_;
			const Caster<Ts...> caster_;

			const std::string functionName_;
			std::vector<Expression> arguments_;
		};

	}
}