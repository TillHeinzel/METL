#pragma once

#include "src/OperationApplier.hpp"

namespace metl
{
	namespace internal
	{
		template <class ... Ts>
		OperationApplier<Ts...>::OperationApplier(const CompilerEntityDataBase<Ts...>& dataBase) : dataBase_(dataBase),
			caster_(dataBase)
		{}

		template <class ... Ts>
		UntypedExpression<Ts...> applyTo(const UntypedConversion<UntypedExpression<Ts...>>& implementation,
										 const std::vector<UntypedExpression<Ts...>>& arguments)
		{
			assert(arguments.size() == 1);
			return implementation.apply(arguments.front());
		}

		template <class ... Ts>
		UntypedExpression<Ts...> applyTo(const UntypedFunction<UntypedExpression<Ts...>>& implementation,
										 const std::vector<UntypedExpression<Ts...>>& arguments)
		{
			return implementation.apply(arguments);
		}

		template<class... Ts>
		std::vector<TYPE> getTypes(const std::vector<UntypedExpression<Ts...>>& expressions)
		{
			return get_each(expressions, [](const auto& expr)
			{
				return expr.type();
			});
		}

		template <class ... Ts>
		template <class IDLabel>
		UntypedExpression<Ts...> OperationApplier<Ts...>::apply(const OperationID<IDLabel>& id,
																const std::vector<Expression>& arguments) const
		{
			const auto inTypes = getTypes(arguments);
			auto sig = findValidSignature(id, inTypes);
			auto castedArguments = castIfNecessary(arguments, sig.argumentTypes);

			auto implementation = findImpl(sig);
			return applyTo(implementation, castedArguments);

		}

		template <class ... Ts>
		template <class IDLabel>
		OperationSignature<IDLabel> OperationApplier<Ts...>::findValidSignature(const OperationID<IDLabel>& id,
																				const std::vector<TYPE>& argTypes) const
		{
			auto sig = makeSignature(id, argTypes);

			const auto implementationOpt = dataBase_.find(sig);

			if(implementationOpt)
			{
				return sig;
			}

			return caster_.findNonAmbiguousConvertedSignature(id, argTypes);
		}

		template <class ... Ts>
		std::vector<typename OperationApplier<Ts...>::Expression> OperationApplier<Ts...>::castIfNecessary(
			const std::vector<Expression>& expressions, const std::vector<TYPE> types) const
		{
			if(getTypes(expressions) == types)
			{
				return expressions;
			}
			return caster_.castTo(expressions, types);
		}
	}
}
