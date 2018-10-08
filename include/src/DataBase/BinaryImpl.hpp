#pragma once

#include "src/DataBase/OperationImpl.hpp"
#include "src/DataBase/BinaryID.hpp"

#include "src/TypeErasure/UntypedFunction.hpp"
#include "src/TypeErasure/UntypedExpression.hpp"

namespace metl
{
	namespace internal
	{
		template<class... Ts>
		struct OperationImpl<BinaryLabel, Ts...>
		{
			UntypedFunction<UntypedExpression<Ts...>> impl;
		};

		template<class... Ts>
		using BinaryImpl = OperationImpl<BinaryLabel, Ts...>;
	}
}
