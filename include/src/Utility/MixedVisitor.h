#pragma once

#include "src/Utility/ConstexprBranching.h"
#include "src/std17/remove_cvref.h"
#include "src/Utility/is_callable.h"

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
				auto argTuple = std::tuple<decltype(ts)...>(std::forward<Ts>(ts)...);
				return constexpr_ternary(
					std17::bool_constant<is_callable_v<FixedTypeCase, Arguments<Ts...>>>(),
					[&, &fixedTypeCase = fixedTypeCase_](auto _)
				{
					return std17::apply(_(fixedTypeCase), argTuple);
				},
					[&, &genericCase = genericCase_](auto _)
				{
					return std17::apply(_(genericCase), argTuple);
				}
				);
			}

		private:
			FixedTypeCase fixedTypeCase_;
			GenericCase genericCase_;
		};

		template<class FixedTypeCase, class GenericCase>
		MixedVisitor<std17::remove_cvref_t<FixedTypeCase>, std17::remove_cvref_t<GenericCase>> makeMixedVisitor(FixedTypeCase&& fixedTypeCase, GenericCase&& genericCase)
		{
			return {std::forward<FixedTypeCase>(fixedTypeCase), std::forward<GenericCase>(genericCase)};
		}

	}
}
