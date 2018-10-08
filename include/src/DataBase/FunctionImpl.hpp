#pragma once

#include "src/DataBase/OperationImpl.hpp"
#include "src/DataBase/FunctionID.hpp"

#include "src/TypeErasure/UntypedFunction.hpp"
#include "src/TypeErasure/UntypedExpression.hpp"

namespace metl
{
	namespace internal
	{
		template<class... Ts>
		struct OperationImpl<FunctionLabel, Ts...>
		{
			UntypedFunction<UntypedExpression<Ts...>> impl;
		};

		template<class... Ts>
		using FunctionImpl = OperationImpl<FunctionLabel, Ts...>;
	}
}
