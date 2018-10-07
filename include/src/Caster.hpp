#pragma once

#include "src/CompilerEntityDataBase.h"

#include "src/StackHelpers.h"

#include "src/UnaryID.h"
#include "src/BinaryID.h"
#include "src/FunctionID.h"
#include "src/SuffixID.h"

namespace metl
{
	namespace internal
	{
		template<class... Ts>
		class Caster
		{
			using Expression = UntypedExpression<Ts...>;
		public:

			explicit Caster(const CompilerEntityDataBase<Ts...>& dataBase) : dataBase_(dataBase)
			{}

			template<class ID>
			std::vector<TYPE> findNonAmbiguousConversionTarget(const ID& id, const std::vector<TYPE>& inTypes) const;

			std::vector<Expression> castTo(const std::vector<Expression>& expressions, const std::vector<TYPE>& targetTypes) const;

		private:
			template<class CheckExistence>
			std::vector<std::vector<TYPE>> getValidCasts(const std::vector<TYPE> inTypes, const CheckExistence& doTypesWork) const;

			auto findImpl(const SuffixID& id, const std::vector<TYPE>& types) const
			{
				assert(types.size() == 1);
				auto castedName = mangleSuffix(id.name, types.front());
				return dataBase_.findSuffix(castedName);
			}

			auto findImpl(const UnaryID& id, const std::vector<TYPE>& types) const
			{
				assert(types.size() == 1);
				auto castedName = mangleName(id.name, types);
				return dataBase_.findOperator(castedName);
			}
			auto findImpl(const FunctionID& id, const std::vector<TYPE>& types) const
			{
				auto castedName = mangleName(id.name, types);
				return dataBase_.findFunction(castedName);
			}
			auto findImpl(const BinaryID& id, const std::vector<TYPE>& types) const
			{
				auto castedName = mangleName(id.name, types);
				return dataBase_.findOperator(castedName);
			}

			static std::string toString(const SuffixID& id) 
			{
				return "suffix " + id.name;
			}
			static std::string toString(const UnaryID& id)
			{
				return "unary operator " + id.name;
			}
			static std::string toString(const FunctionID& id)
			{
				return "function " + id.name;
			}
			static std::string toString(const BinaryID& id)
			{
				return "binary operator " + id.name;
			}

			const CompilerEntityDataBase<Ts...>& dataBase_;;
		};

	}
}