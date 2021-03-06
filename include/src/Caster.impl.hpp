#pragma once

#include "src/Caster.hpp"

#include "src/Utility/applyOnEachElement.hpp"

namespace metl
{
	namespace internal
	{
		template<class T>
		void throwIfNotExactlyOneValidCast(const std::vector<T>& validCasts, const std::string& objectLabel)
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
		OperationSignature<IDLabel> Caster<Ts...>::findNonAmbiguousConvertedSignature(const OperationID<IDLabel>& id, const std::vector<TYPE>& inTypes) const
		{
			auto inSignature = makeSignature(id, inTypes);

			auto candidateSignatures = generateConceivableSignatures(inSignature);
			auto validSignatures = excludeNonExistingSignatures(candidateSignatures);

			throwIfNotExactlyOneValidCast(validSignatures, labelString<IDLabel>() + " " + id.name);
			return validSignatures.front();
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
		template <class IDLabel>
		std::vector<OperationSignature<IDLabel>> Caster<Ts...>::excludeNonExistingSignatures(const std::vector<OperationSignature<IDLabel>>& conceivableSignatures) const
		{
			std::vector<OperationSignature<IDLabel>> validCasts;
			for(auto signature : conceivableSignatures)
			{
				if(doesImplementationExist(signature))
				{
					validCasts.push_back(signature);
				}
			}
			return validCasts;
		}

		template <class... Ts>
		template<class IDLabel>
		std::vector<OperationSignature<IDLabel>> Caster<Ts...>::generateConceivableSignatures(const OperationSignature<IDLabel>& signature) const
		{
			auto inTypes = signature.argumentTypes;

			std::vector<std::vector<TYPE>> castCombis{{}};
			for(auto t : inTypes)
			{
				castCombis = tensorSum(castCombis, dataBase_.getAllTypesCastableFrom(t));
			}

			auto makeSig = [&name = signature.name](const auto& castCombi)
			{
				return makeSignature(OperationID<IDLabel>{name}, castCombi);
			};
			return applyOnEachElement(makeSig, castCombis);
		}

		template <class ... Ts>
		template<class IDLabel>
		bool Caster<Ts...>::doesImplementationExist(const OperationSignature<IDLabel>& signature) const
		{
			auto castedImplOpt = dataBase_.find(signature);
			return castedImplOpt.has_value();
		}

		template <class ... Ts>
		UntypedExpression<Ts...> Caster<Ts...>::castIfNecessary(const UntypedExpression<Ts...>& expr, TYPE toType) const
		{
			auto fromType = expr.type();
			return fromType == toType ? expr : dataBase_.findCast(mangleCast(fromType, toType))->apply(expr);
		}
	}


}
