#pragma once

#include "src/CompilerEntityDataBase.h"

#include "src/StackHelpers.h"

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

			TYPE findTypeForUnaryOperator(const std::string& opName, TYPE inType) const;
			TYPE findTypeForSuffix(const std::string& suffix, TYPE inType) const;

			std::vector<TYPE> findTypesForFunction(const std::string& functionName, const std::vector<TYPE>& inTypes) const;
			std::vector<TYPE> findTypesForBinaryOperator(const std::string& opName, const std::vector<TYPE>& inTypes) const;

			void castTo(std::vector<Expression>& expressions, const std::vector<TYPE>& targetTypes) const;

		private:
			template<class CheckExistence>
			std::vector<TYPE> getValidCasts(const TYPE inType, const CheckExistence& doesTypeWork) const;

			template<class CheckExistence>
			std::vector<std::vector<TYPE>> getValidCasts(const std::vector<TYPE> inTypes, const CheckExistence& doTypesWork) const;


			const CompilerEntityDataBase<Ts...>& dataBase_;;
		};

	}
}