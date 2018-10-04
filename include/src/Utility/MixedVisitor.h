#pragma once

#include "src/Utility/ConstexprBranching.h"

namespace metl
{
	namespace internal
	{
		template<class FixedTypeCase, class GenericCase>
		class MixedVisitor
		{
		public:
			MixedVisitor(FixedTypeCase fixedTypeCase, GenericCase genericCase) :
				fixedTypeCase_(fixedTypeCase),
				genericCase_(genericCase)
			{}

			template<class... Ts>
			decltype(auto) operator() (Ts&&... ts)
			{
				//return constexpr_ternary(is_callable_v<FixedTypeCase>)
				return genericCase_(std::forward<Ts>(ts)...);
			}

		private:
			FixedTypeCase fixedTypeCase_;
			GenericCase genericCase_;
		};

		template<class FixedTypeCase, class GenericCase>
		MixedVisitor<FixedTypeCase, GenericCase> makeMixedVisitor(FixedTypeCase&& fixedTypeCase, GenericCase&& genericCase)
		{
			return {std::forward<FixedTypeCase>(fixedTypeCase), std::forward<GenericCase>(genericCase)};
		}

	}
}
