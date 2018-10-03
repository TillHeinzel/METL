

#pragma once

#include "src/TypeErasure/UntypedVariable.h"

#include "src/TypeErasure/UntypedExpression.h"

#include "src/std17/remove_cvref.h"

namespace metl
{
	template <class ... Ts>
	template <class T>
	UntypedVariable<Ts...>::UntypedVariable(T* var): variable_(var)
	{
	}

	template <class ... Ts>
	template <class T>
	void UntypedVariable<Ts...>::setValue(const T& t)
	{
		auto visitor = [&t](auto* typedVariablePtr)
		{
			*typedVariablePtr = t;
		};
		mpark::visit(visitor, variable_);
	}

	template <class ... Ts>
	UntypedExpression<Ts...> UntypedVariable<Ts...>::makeUntypedExpression() const
	{
		auto visitor = [](const auto* typedVariablePtr)
		{
			auto valueLambda = [typedVariablePtr]()
			{
				return *typedVariablePtr;
			};

			return UntypedExpression<Ts...>::makeNonConstexpr(valueLambda);
		};
		return mpark::visit(visitor, variable_);
	}

	template <class ... Ts>
	TYPE UntypedVariable<Ts...>::type() const
	{
		auto visitor = [](const auto* typedValue)
		{
			using T = std17::remove_cvref_t<std::remove_pointer_t<std17::remove_cvref_t<decltype(typedValue)>>>;

			return classToType2<T, Ts...>();
		};
		return mpark::visit(visitor, variable_);
	}
}
