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

			std::vector<TYPE> findNonAmbiguousConversionTarget(const UnaryID& id, const std::vector<TYPE>& inTypes) const;
			std::vector<TYPE> findNonAmbiguousConversionTarget(const SuffixID& id, const std::vector<TYPE>& inTypes) const;
			std::vector<TYPE> findNonAmbiguousConversionTarget(const FunctionID& id, const std::vector<TYPE>& inTypes) const;
			std::vector<TYPE> findNonAmbiguousConversionTarget(const BinaryID& id, const std::vector<TYPE>& inTypes) const;

			std::vector<Expression> castTo2(const std::vector<Expression>& expressions, const std::vector<TYPE>& targetTypes) const;

		private:
			template<class CheckExistence>
			std::vector<TYPE> getValidCasts(const TYPE inType, const CheckExistence& doesTypeWork) const;

			template<class CheckExistence>
			std::vector<std::vector<TYPE>> getValidCasts(const std::vector<TYPE> inTypes, const CheckExistence& doTypesWork) const;


			const CompilerEntityDataBase<Ts...>& dataBase_;;
		};

	}
}