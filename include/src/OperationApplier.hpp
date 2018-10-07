#pragma once

#include "src/Utility/get_each.hpp"

#include "src/CompilerEntityDataBase.hpp"
#include "src/Caster.hpp"

#include "src/UnaryID.hpp"
#include "src/BinaryID.hpp"
#include "src/FunctionID.hpp"
#include "src/SuffixID.hpp"

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

			template<class IDLabel>
			UntypedExpression<Ts...> apply(const OperationID<IDLabel>& id, const std::vector<Expression>& arguments) const;

		private:
			const CompilerEntityDataBase<Ts...>& dataBase_;
			const Caster<Ts...> caster_;
		};
	}
}
