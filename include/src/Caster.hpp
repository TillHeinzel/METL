#pragma once

#include "src/CompilerEntityDataBase.hpp"

#include "src/StackHelpers.hpp"

#include "src/UnaryID.hpp"
#include "src/BinaryID.hpp"
#include "src/FunctionID.hpp"
#include "src/SuffixID.hpp"

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

			template<class IDLabel>
			std::vector<TYPE> findNonAmbiguousConversionTarget(const OperationID<IDLabel>& id, const std::vector<TYPE>& inTypes) const;

			std::vector<Expression> castTo(const std::vector<Expression>& expressions, const std::vector<TYPE>& targetTypes) const;

		private:
			template<class CheckExistence>
			std::vector<std::vector<TYPE>> getValidCasts(const std::vector<TYPE> inTypes, const CheckExistence& doTypesWork) const;
			
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

			UntypedExpression<Ts...> castIfNecessary(const UntypedExpression<Ts...>& expr, TYPE toType) const;

			const CompilerEntityDataBase<Ts...>& dataBase_;;
		};

	}
}
