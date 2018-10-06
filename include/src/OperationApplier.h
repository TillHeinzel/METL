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

			UntypedExpression<Ts...> applySuffix(const std::string& suffix, const Expression& argument) const;

			UntypedExpression<Ts...> applyUnary(const std::string& opName, const Expression& argument) const;

			template<class ID>
			UntypedExpression<Ts...> apply(const ID& id, const Expression& argument) const;

			UntypedExpression<Ts...> applyBinary(const std::string& opName,
			                                     const std::vector<Expression>& arguments) const;

			UntypedExpression<Ts...> applyFunction(const std::string& functionName,
			                                       const std::vector<Expression>& arguments) const;


			template<class ID>
			UntypedExpression<Ts...> apply(const ID& id, const std::vector<Expression>& arguments) const;



		private:
			auto findImpl(const SuffixID& id, TYPE inType) const
			{
				return dataBase_.findSuffix(mangleSuffix(id.name, {inType}));
			}


			TYPE uniqueTargetType(const SuffixID& id, TYPE inType) const
			{
				return caster_.findTypeForSuffix(id.name, inType);
			}
			
			auto findImpl(const UnaryID& id, TYPE inType) const
			{
				return dataBase_.findOperator(mangleName(id.name, {inType}));
			}
			TYPE uniqueTargetType(const UnaryID& id, TYPE inType) const
			{
				return caster_.findTypeForUnaryOperator(id.name, inType);
			}


			auto findImpl(const BinaryID& id, const std::vector<TYPE>& inTypes) const
			{
				return dataBase_.findOperator(mangleName(id.name, inTypes));
			}

			std::vector<TYPE> uniqueTargetTypes(const BinaryID& id, std::vector<TYPE> inTypes) const
			{
				return caster_.findTypesForBinaryOperator(id.name, inTypes);
			}
			
			auto findImpl(const FunctionID& id, const std::vector<TYPE>& inTypes) const
			{
				return dataBase_.findFunction(mangleName(id.name, inTypes));
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
