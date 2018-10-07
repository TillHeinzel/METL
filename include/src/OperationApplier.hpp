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

			template<class ID>
			UntypedExpression<Ts...> apply(const ID& id, const std::vector<Expression>& arguments) const;

		private:
			UntypedExpression<Ts...> applyTo(const UntypedConversion<Expression>& implementation, const std::vector<Expression>& arguments) const
			{
				assert(arguments.size() == 1);
				return implementation.apply(arguments.front());
			}

			UntypedExpression<Ts...> applyTo(const UntypedFunction<Expression>& implementation, const std::vector<Expression>& arguments) const
			{
				return implementation.apply(arguments);
			}

			auto findImpl(const SuffixID& id, const std::vector<TYPE>& inTypes) const
			{
				assert(inTypes.size() == 1);
				return dataBase_.findSuffix(mangleSuffix(id.name, inTypes.front()));
			}
			auto findImpl(const UnaryID& id, const std::vector<TYPE>& inTypes) const
			{
				assert(inTypes.size() == 1);
				return dataBase_.findOperator(mangleName(id.name, inTypes));
			}
			auto findImpl(const BinaryID& id, const std::vector<TYPE>& inTypes) const
			{
				return dataBase_.findOperator(mangleName(id.name, inTypes));
			}
			auto findImpl(const FunctionID& id, const std::vector<TYPE>& inTypes) const
			{
				return dataBase_.findFunction(mangleName(id.name, inTypes));
			}

			const CompilerEntityDataBase<Ts...>& dataBase_;
			const Caster<Ts...> caster_;
		};
	}
}
