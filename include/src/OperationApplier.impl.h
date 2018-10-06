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
		UntypedExpression<Ts...> OperationApplier<Ts...>::applySuffix(const std::string& suffix,
		                                                              const Expression& argument) const
		{
			return apply(SuffixID{suffix}, argument);
		}

		template <class ... Ts>
		UntypedExpression<Ts...> OperationApplier<Ts...>::applyUnary(const std::string& opName,
		                                                             const Expression& argument) const
		{
			return apply(UnaryID{opName}, argument);
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
		UntypedExpression<Ts...> OperationApplier<Ts...>::applyBinary(const std::string& opName,
		                                                              const std::vector<Expression>& arguments) const
		{
			return apply(BinaryID{opName}, arguments);
		}

		template <class ... Ts>
		UntypedExpression<Ts...> OperationApplier<Ts...>::applyFunction(const std::string& functionName,
		                                                                const std::vector<Expression>& arguments) const
		{
			return apply(FunctionID{functionName}, arguments);
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
