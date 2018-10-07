#pragma once

#include "src/Caster.h"

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
		TYPE Caster<Ts...>::findTypeForUnaryOperator(const std::string& opName, const TYPE inType) const
		{
			auto doesTypeWork = [&opName, &database = dataBase_](TYPE t) -> bool
			{
				auto castedName = mangleName(opName, {t});
				auto castedImplOpt = database.findOperator(castedName);

				return castedImplOpt.has_value();
			};

			auto validCasts = getValidCasts(inType, doesTypeWork);

			checkExactlyOneValidCast(validCasts, "unary operator " + opName);
			return validCasts.back();
		}

		template <class ... Ts>
		TYPE Caster<Ts...>::findTypeForSuffix(const std::string& suffix, const TYPE inType) const
		{
			auto doesTypeWork = [&suffix, &database = dataBase_](TYPE t) -> bool
			{
				auto castedName = mangleSuffix(suffix, t);
				auto castedImplOpt = database.findSuffix(castedName);

				return castedImplOpt.has_value();
			};

			auto validCasts = getValidCasts(inType, doesTypeWork);

			checkExactlyOneValidCast(validCasts, "suffix " + suffix);
			return validCasts.back();
		}


		template <class ... Ts>
		std::vector<TYPE> Caster<Ts...>::findTypesForFunction(const std::string& functionName, const std::vector<TYPE>& inTypes) const
		{
			auto doTypesWork = [&functionName, &database = dataBase_](const std::vector<TYPE>& toTypes) -> bool
			{
				auto mangledName = mangleName(functionName, toTypes);
				auto castedImplOpt = database.findFunction(mangledName);

				return castedImplOpt.has_value();
			};

			auto validCasts = getValidCasts(inTypes, doTypesWork);

			checkExactlyOneValidCast(validCasts, "function " + functionName);
			return validCasts.back();

		}

		template <class ... Ts>
		std::vector<TYPE> Caster<Ts...>::findTypesForBinaryOperator(const std::string& opName, const std::vector<TYPE>& inTypes) const
		{
			auto doTypesWork = [&opName, &database = dataBase_](const std::vector<TYPE>& toTypes) -> bool
			{
				auto castedName = mangleName(opName, toTypes);
				auto castedImplOpt = database.findOperator(castedName);

				return castedImplOpt.has_value();
			};

			auto validCasts = getValidCasts(inTypes, doTypesWork);

			checkExactlyOneValidCast(validCasts, "operator " + opName);
			return validCasts.back();
		}

		template <class ... Ts>
		std::vector<typename Caster<Ts...>::Expression> Caster<Ts...>::castTo2(
			const std::vector<Expression>& expressions, const std::vector<TYPE>& targetTypes) const
		{
			assert(expressions.size() == targetTypes.size());

			auto exprIt = expressions.cbegin();
			auto targetTypesIt = targetTypes.cbegin();

			std::vector<Expression> retval;
			retval.reserve(expressions.size());

			for(; exprIt != expressions.cend(); ++exprIt, ++targetTypesIt)
			{
				const auto& expr = *exprIt;
				const auto fromType = expr.type();

				const auto& toType = *targetTypesIt;

				retval.push_back(fromType == toType ?
								 expr :
								 dataBase_.findCast(mangleCast(fromType, toType))->apply(expr)

				);
			}

			return expressions;
		}

		template <class ... Ts>
		template <class CheckExistence>
		std::vector<TYPE> Caster<Ts...>::getValidCasts(const TYPE inType, const CheckExistence& doesTypeWork) const
		{
			std::vector<TYPE> allowedCasts = dataBase_.getAllTypesCastableFrom(inType);

			std::vector<TYPE> validCasts;
			for(auto c : allowedCasts)
			{
				if(doesTypeWork(c))
				{
					validCasts.push_back(c);
				}
			}

			return validCasts;
		}

		template <class ... Ts>
		template <class CheckExistence>
		std::vector<std::vector<TYPE>> Caster<Ts...>::getValidCasts(const std::vector<TYPE> inTypes, const CheckExistence& doTypesWork) const
		{
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

	}


}