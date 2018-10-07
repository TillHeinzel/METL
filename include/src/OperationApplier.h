#pragma once

#include "src/Utility/get_each.h"

#include "src/CompilerEntityDataBase.h"
#include "src/Caster.h"

#include "src/UnaryID.h"
#include "src/BinaryID.h"
#include "src/FunctionID.h"
#include "src/SuffixID.h"

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

			std::vector<TYPE> uniqueTargetTypes(const SuffixID& id, const std::vector<TYPE>& inTypes) const
			{
				assert(inTypes.size() == 1);
				return {caster_.findTypeForSuffix(id.name, inTypes.front())};
			}

			std::vector<TYPE> uniqueTargetTypes(const UnaryID& id, const std::vector<TYPE>& inTypes) const
			{
				assert(inTypes.size() == 1);
				return {caster_.findTypeForUnaryOperator(id.name, inTypes.front())};
			}

			std::vector<TYPE> uniqueTargetTypes(const BinaryID& id, std::vector<TYPE> inTypes) const
			{
				return caster_.findTypesForBinaryOperator(id.name, inTypes);
			}

			std::vector<TYPE> uniqueTargetTypes(const FunctionID& id, std::vector<TYPE> inTypes) const
			{
				return caster_.findTypesForFunction(id.name, inTypes);
			}



			const CompilerEntityDataBase<Ts...>& dataBase_;
			const Caster<Ts...> caster_;
		};
	}
}
