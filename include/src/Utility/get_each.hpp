#pragma once
#include <vector>


namespace metl
{
	namespace internal
	{
		template<class T, class F>
		auto get_each(const std::vector<T>& source, const F& f)
		{
			std::vector<std::result_of_t<F(T)>> retval;
			retval.reserve(source.size());
			for(const auto& x: source)
			{
				retval.push_back(f(x));
			}
			return retval;
		}
	}
}
