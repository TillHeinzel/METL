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
			const auto vecTup = std::tuple<const std::vector<Ts>&...>(vs...);

			auto beginLambda = [](const auto& vec)
			{
				return vec.cbegin();
			};

			auto itTuple = evaluate_each(beginLambda, vecTup);

			auto size = std::get<0>(vecTup).size();

			auto assertCorrectSize = [size](const auto& vec)
			{
				assert(vec.size() == size);
			};

			std::vector<RetT> retval;
			retval.reserve(size);

			auto iterate = [](auto& it)
			{
				++it;
			};

			auto dereference = [](const auto& it) -> decltype(*it)
			{
				return *it;
			};

			for(auto i = 0u; i<size ; apply_each(iterate, itTuple), ++i)
			{
				auto refTuple = evaluate_each(dereference, itTuple);
				retval.push_back(std17::apply(f, refTuple));
			}

			return retval;
		}
	}
}