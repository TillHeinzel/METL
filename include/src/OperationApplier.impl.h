#pragma once

#include "src/OperationApplier.h"

namespace metl
{
	namespace internal
	{
		template <class ... Ts>
		OperationApplier<Ts...>::OperationApplier(const CompilerEntityDataBase<Ts...>& dataBase): dataBase_(dataBase),
		                                                                                          caster_(dataBase)
		{
		}

		template <class ... Ts>
		template <class ID>
		UntypedExpression<Ts...> OperationApplier<Ts...>::apply(const ID& id,
			const std::vector<Expression>& arguments) const
		{
			const auto inTypes = get_each(arguments, [](const auto& expr)
			{
				return expr.type();
			});
			const auto implementationOpt = findImpl(id, inTypes);

			if (!implementationOpt)
			{
				const auto targetTypes = caster_.findNonAmbiguousConversionTarget(id, inTypes);
				const auto castedArguments = caster_.castTo2(arguments, targetTypes);
				return apply(id, castedArguments); // call recursively
			}

			return applyTo(*implementationOpt, arguments);
		}
	}
}
