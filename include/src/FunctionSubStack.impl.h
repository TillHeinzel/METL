#pragma once
#include "src/FunctionSubStack.h"

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
		void FunctionSubStack<Ts...>::push(Expression l)
		{
			arguments_.push_back(std::move(l));
		}

		template <class ... Ts>
		typename FunctionSubStack<Ts...>::Expression FunctionSubStack<Ts...>::finish()
		{
			evaluateFunction();

			assert(arguments_.size() == 1);

			const auto r = arguments_.back();
			arguments_.clear();
			return r;
		}

		template <class ... Ts>
		bool FunctionSubStack<Ts...>::empty() const
		{
			return arguments_.empty();
		}

		template <class ... Ts>
		void FunctionSubStack<Ts...>::evaluateFunction()
		{
			auto inTypes = getTypes(arguments_);

			auto functionOpt = dataBase_.findFunction(mangleName(functionName_, inTypes));
			if (!functionOpt)
			{
				auto targetTypes = caster_.findTypesForFunction(functionName_, inTypes);
				caster_.castTo(arguments_, targetTypes);
				evaluateFunction(); // call recursively
			}

			auto resultExpression = functionOpt->apply(arguments_);

			arguments_.clear();
			arguments_.push_back(resultExpression);
		}
	}
}