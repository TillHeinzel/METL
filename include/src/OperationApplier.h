#pragma once

#include "src/Utility/get_each.h"

#include "src/CompilerEntityDataBase.h"
#include "src/Caster.h"

namespace metl
{
	namespace internal
	{
		template<class... Ts>
		class OperationApplier
		{
			using Expression = UntypedExpression<Ts...>;
		public:
			OperationApplier(const CompilerEntityDataBase<Ts...>& dataBase);

			UntypedExpression<Ts...> applySuffix(const std::string& suffix, const Expression& argument) const;

			UntypedExpression<Ts...> applyUnary(const std::string& opName, const Expression& argument) const;

			UntypedExpression<Ts...> applyBinary(const std::string& opName,
			                                     const std::vector<Expression>& arguments) const;

			UntypedExpression<Ts...> applyFunction(const std::string& functionName,
			                                       const std::vector<Expression>& arguments) const;

		private:

			const CompilerEntityDataBase<Ts...>& dataBase_;
			const Caster<Ts...> caster_;
		};

	}
}
