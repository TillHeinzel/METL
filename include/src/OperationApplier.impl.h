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
		UntypedExpression<Ts...> OperationApplier<Ts...>::apply(const ID& id, const Expression& argument) const
		{
			const auto inType = argument.type();
			const auto implementationOpt = findImpl(id, inType);

			if(!implementationOpt)
			{
				const auto targetType = uniqueTargetType(id, inType);
				const auto castedArgument = caster_.castTo2({argument}, {targetType});
				return apply(id, castedArgument.front()); // call recursively
			}

			return implementationOpt->apply({argument});
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
				const auto targetTypes = uniqueTargetTypes(id, inTypes);
				const auto castedArguments = caster_.castTo2(arguments, targetTypes);
				return apply(id, castedArguments); // call recursively
			}

			return implementationOpt->apply(arguments);
		}
	}
}
