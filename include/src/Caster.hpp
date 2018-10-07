#pragma once

#include <vector>

#include "src/DataBase/CompilerEntityDataBase.hpp"

#include "src/StackHelpers.hpp"

#include "src/DataBase/UnaryID.hpp"
#include "src/DataBase/BinaryID.hpp"
#include "src/DataBase/FunctionID.hpp"
#include "src/DataBase/SuffixID.hpp"

#include "src/DataBase/OperationSignature.hpp"

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
			OperationSignature<IDLabel> findNonAmbiguousConvertedSignature(const OperationID<IDLabel>& id, const std::vector<TYPE>& inTypes) const;

			std::vector<Expression> castTo(const std::vector<Expression>& expressions, const std::vector<TYPE>& targetTypes) const;

		private:
			template<class IDLabel>
			std::vector<std::vector<TYPE>> getValidCasts(const OperationID<IDLabel>& id, const std::vector<TYPE>& inTypes) const;

			std::vector<std::vector<TYPE>> getConceivableCasts( const std::vector<TYPE>& inTypes) const;


			template<class IDLabel>
			bool doesImplementationExist(const OperationID<IDLabel>& id, const std::vector<TYPE>& types) const;

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
