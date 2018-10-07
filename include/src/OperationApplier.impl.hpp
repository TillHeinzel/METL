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

		template <class ... Ts>
		template <class IDLabel>
		UntypedExpression<Ts...> OperationApplier<Ts...>::apply(const OperationID<IDLabel>& id,
																const std::vector<Expression>& arguments) const
		{
			const auto inTypes = get_each(arguments, [](const auto& expr)
			{
				return expr.type();
			});
			const auto implementationOpt = dataBase_.find(makeSignature(id, inTypes));

			if(!implementationOpt)
			{
				const auto targetTypes = caster_.findNonAmbiguousConversionTarget(id, inTypes);
				const auto castedArguments = caster_.castTo(arguments, targetTypes);
				return apply(id, castedArguments); // call recursively
			}

			return applyTo(*implementationOpt, arguments);
		}

	}
}
