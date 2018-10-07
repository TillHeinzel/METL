#pragma once

#include "src/Utility/get_each.hpp"

#include "src/TypeErasure/UntypedExpression.hpp"

#include "src/DataBase/CompilerEntityDataBase.hpp"
#include "src/Caster.hpp"

#include "src/DataBase/UnaryID.hpp"
#include "src/DataBase/BinaryID.hpp"
#include "src/DataBase/FunctionID.hpp"
#include "src/DataBase/SuffixID.hpp"

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

			template<class IDLabel>
			OperationSignature<IDLabel> findValidSignature(const OperationID<IDLabel>& id, const std::vector<TYPE>& argTypes)const;

			std::vector<Expression> castIfNecessary(const std::vector<Expression>& expressions, const std::vector<TYPE> types) const;

			template<class IDLabel>
			auto findImpl(const OperationSignature<IDLabel>& sig) const
			{
				const auto implementationOpt = dataBase_.find(sig);
				assert(implementationOpt);
				return *implementationOpt;
			}

			const CompilerEntityDataBase<Ts...>& dataBase_;
			const Caster<Ts...> caster_;
		};
	}
}
