#pragma once

#include "../Utility/apply_each.h"

namespace metl
{
	namespace internal
	{
		template<class Tuple>
		decltype(auto) evaluate_each(Tuple&& callables)
		{
			return internal::apply_each([](const auto& f) {return f(); }, std::forward<Tuple>(callables));
		}
	}
}
