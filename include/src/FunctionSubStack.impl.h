#pragma once
#include "src/FunctionSubStack.h"

#include "src/Utility/get_each.h"

namespace metl
{
	namespace internal
	{
		template <class ... Ts>
		FunctionSubStack<Ts...>::FunctionSubStack(const CompilerEntityDataBase<Ts...>& database, std::string functionName):
			dataBase_(database),
			caster_(database),
			functionName_(std::move(functionName))
		{
		}

		template <class ... Ts>
		void FunctionSubStack<Ts...>::addArgument(Expression l)
		{
			arguments_.push_back(std::move(l));
		}

		template <class ... Ts>
		typename FunctionSubStack<Ts...>::Expression FunctionSubStack<Ts...>::finish()
		{
			const auto r = applyFunction(functionName_, arguments_);
			arguments_.clear();
			return r;
		}

		template <class ... Ts>
		bool FunctionSubStack<Ts...>::empty() const
		{
			return arguments_.empty();
		}

		template <class ... Ts>
		UntypedExpression<Ts...> FunctionSubStack<Ts...>::applyFunction(const std::string& functionName,
			const std::vector<Expression>& arguments) const
		{
			auto inTypes = get_each(arguments, [](const auto& expr)
			{
				return expr.type();
			});
			auto functionImplOpt = dataBase_.findFunction(mangleName(functionName, inTypes));

			if(!functionImplOpt)
			{
				auto targetTypes = caster_.findTypesForFunction(functionName, inTypes);
				auto castedArguments = caster_.castTo2(arguments, targetTypes);
				return applyFunction(functionName, castedArguments); // call recursively
			}

			return functionImplOpt->apply(arguments);
		}
	}
}
