#pragma once

#include "src/DataBase/OperationImpl.hpp"
#include "src/DataBase/UnaryID.hpp"

#include "src/TypeErasure/UntypedFunction.hpp"
#include "src/TypeErasure/UntypedExpression.hpp"

namespace metl
{
	namespace internal
	{
		template<class... Ts>
		struct OperationImpl<UnaryLabel, Ts...>
		{
			UntypedFunction<UntypedExpression<Ts...>> impl;
		};

		template<class... Ts>
		using UnaryImpl = OperationImpl<UnaryLabel, Ts...>;
	}
}
