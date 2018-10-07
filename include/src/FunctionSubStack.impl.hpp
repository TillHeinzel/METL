#pragma once
#include "src/FunctionSubStack.hpp"

namespace metl
{
	namespace internal
	{
		template <class ... Ts>
		FunctionSubStack<Ts...>::FunctionSubStack(const CompilerEntityDataBase<Ts...>& dataBase, std::string functionName):
			operationApplier_(dataBase),
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
			const auto r = operationApplier_.apply(FunctionID{functionName_}, arguments_);
			arguments_.clear();
			return r;
		}

		template <class ... Ts>
		bool FunctionSubStack<Ts...>::empty() const
		{
			return arguments_.empty();
		}
	}
}
