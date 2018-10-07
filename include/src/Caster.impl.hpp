#pragma once

#include "src/Caster.hpp"
#include "src/Utility/applyOnEachElement.hpp"

namespace metl
{
	namespace internal
	{
		template<class T>
		void checkExactlyOneValidCast(const std::vector<T>& validCasts, const std::string& objectLabel)
		{
			if(validCasts.size() > 1)
			{
				throw std::runtime_error("To many possible overloads for " + objectLabel);
			}

			if(validCasts.size() == 0)
			{
				throw std::runtime_error("could not find a match for " + objectLabel);
			}
		}

		template <class ... Ts>
		template <class IDLabel>
		std::vector<TYPE> Caster<Ts...>::findNonAmbiguousConversionTarget(const OperationID<IDLabel>& id, const std::vector<TYPE>& inTypes) const
		{
			auto doTypesWork = [&id, &dataBase = dataBase_](const std::vector<TYPE>& toTypes) -> bool
			{
				auto castedImplOpt = dataBase.find(makeSignature(id, toTypes));
				return castedImplOpt.has_value();
			};

			auto validCasts = getValidCasts(inTypes, doTypesWork);

			checkExactlyOneValidCast(validCasts, toString(id));
			return validCasts.front();
		}

		template <class ... Ts>
		std::vector<typename Caster<Ts...>::Expression> Caster<Ts...>::castTo(
			const std::vector<Expression>& expressions, const std::vector<TYPE>& targetTypes) const
		{
			assert(expressions.size() == targetTypes.size());

			auto getCastedExpression = [this](const auto& expr, const auto& targetType)
			{
				return castIfNecessary(expr, targetType);
			};

			return applyOnEachElement(getCastedExpression, expressions, targetTypes);
		}

		template <class ... Ts>
		template <class CheckExistence>
		std::vector<std::vector<TYPE>> Caster<Ts...>::getValidCasts(const std::vector<TYPE> inTypes, const CheckExistence& doTypesWork) const
		{
			static_assert(is_callable_v<CheckExistence, Return<bool>, Arguments<const std::vector<TYPE>&>>, "");

			std::vector<std::vector<TYPE>> castCombis{{}};
			for(auto t : inTypes)
			{
				castCombis = tensorSum(castCombis, dataBase_.getAllTypesCastableFrom(t));
			}

			std::vector<std::vector<TYPE>> validCasts;
			for(auto c : castCombis)
			{
				if(doTypesWork(c))
				{
					validCasts.push_back(c);
				}
			}

			return validCasts;
		}

		template <class ... Ts>
		UntypedExpression<Ts...> Caster<Ts...>::castIfNecessary(const UntypedExpression<Ts...>& expr, TYPE toType) const
		{
			auto fromType = expr.type();
			return fromType == toType ? expr : dataBase_.findCast(mangleCast(fromType, toType))->apply(expr);
		}
	}


}
