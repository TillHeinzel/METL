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
			const auto inType = argument.type();
			auto suffixImplOpt = dataBase_.findSuffix(mangleSuffix(suffix, {inType}));

			if (!suffixImplOpt)
			{
				auto targetType = caster_.findTypeForSuffix(suffix, inType);
				auto castedArguments = caster_.castTo2({argument}, {targetType});
				return applySuffix(suffix, castedArguments.front());
			}

			return suffixImplOpt->apply({argument});
		}

		template <class ... Ts>
		UntypedExpression<Ts...> OperationApplier<Ts...>::applyUnary(const std::string& opName,
		                                                             const Expression& argument) const
		{
			auto inType = argument.type();
			auto operatorImplOpt = dataBase_.findOperator(mangleName(opName, {inType}));

			if (!operatorImplOpt)
			{
				auto targetType = caster_.findTypeForUnaryOperator(opName, inType);
				auto castedArgument = caster_.castTo2({argument}, {targetType});
				return applyUnary(opName, castedArgument.front()); // call recursively
			}

			return operatorImplOpt->apply({argument});
		}

		template <class ... Ts>
		UntypedExpression<Ts...> OperationApplier<Ts...>::applyBinary(const std::string& opName,
		                                                              const std::vector<Expression>& arguments) const
		{
			auto inTypes = get_each(arguments, [](const auto& expr)
			{
				return expr.type();
			});
			auto operatorImplOpt = dataBase_.findOperator(mangleName(opName, inTypes));

			if (!operatorImplOpt)
			{
				auto targetTypes = caster_.findTypesForBinaryOperator(opName, inTypes);
				auto castedArguments = caster_.castTo2(arguments, targetTypes);
				return applyBinary(opName, castedArguments); // call recursively
			}

			return operatorImplOpt->apply(arguments);
		}

		template <class ... Ts>
		UntypedExpression<Ts...> OperationApplier<Ts...>::applyFunction(const std::string& functionName,
		                                                                const std::vector<Expression>& arguments) const
		{
			auto inTypes = get_each(arguments, [](const auto& expr)
			{
				return expr.type();
			});
			auto functionImplOpt = dataBase_.findFunction(mangleName(functionName, inTypes));

			if (!functionImplOpt)
			{
				auto targetTypes = caster_.findTypesForFunction(functionName, inTypes);
				auto castedArguments = caster_.castTo2(arguments, targetTypes);
				return applyFunction(functionName, castedArguments); // call recursively
			}

			return functionImplOpt->apply(arguments);
		}
	}
}
