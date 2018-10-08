#pragma once

#include "src/DataBase/OperationImpl.hpp"
#include "src/DataBase/SuffixID.hpp"

#include "src/TypeErasure/UntypedConversion.hpp"
#include "src/TypeErasure/UntypedExpression.hpp"

namespace metl
{
	namespace internal
	{
		template<class... Ts>
		struct OperationImpl<SuffixLabel, Ts...>
		{
			UntypedConversion<UntypedExpression<Ts...>> impl;
		};

		template<class... Ts>
		using SuffixImpl = OperationImpl<SuffixLabel, Ts...>;
	}
}
