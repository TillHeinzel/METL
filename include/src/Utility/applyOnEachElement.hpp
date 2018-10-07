#pragma once

#include <type_traits>
#include <vector>
#include <cassert>
#include <tuple>

#include "src/std17/is_same_v.hpp"
#include "src/std17/apply.hpp"

#include "src/Utility/apply_each.hpp"

namespace metl
{
	namespace internal
	{
		template<class F, class... Ts, class RetT = std::result_of_t<F(const Ts&...)>, class = std::enable_if_t<!std17::is_same_v<RetT, void>>>
		std::vector<RetT> applyOnEachElement(const F& f, const std::vector<Ts>... vs)
		{
			auto itTuple = std::make_tuple(vs.cbegin()...);
			auto endIt = std::get<0>(std::make_tuple(vs...)).cend();

			std::vector<RetT> retval;

			auto iterate = [](auto& it)
			{
				++it;
			};

			auto dereference = [](const auto& it) -> decltype(*it)
			{
				return *it;
			};

			for(; std::get<0>(itTuple) != endIt; apply_each(iterate, itTuple))
			{

				auto refTuple = evaluate_each(dereference, itTuple);
				retval.push_back(std17::apply(f, refTuple));
			}

			return retval;
		}
	}
}