#pragma once
#include <memory>
#include <vector>

namespace qengine
{
	namespace internal
	{
		// deep copy of unique_ptr to abstract base class. requires clone() to be implemented in hierarchy.
		template<class T, std::enable_if_t<std::is_abstract<T>::value, int> = 0>
		std::unique_ptr<T> deep_copy(const std::unique_ptr<T>& t_ptr)
		{
			return t_ptr->clone();
		}

		// deep copy of non-abstract type. Calls copy-ctor, so must be copyable type. Watch out for slicing in deep hierarchies.
		template<class T, std::enable_if_t<!std::is_abstract<T>::value, int> = 0>
		std::unique_ptr<T> deep_copy(const std::unique_ptr<T>& t_ptr)
		{
			if(t_ptr) return std::make_unique<T>(*t_ptr);
			// deep copy of nullptr is nullptr
			return std::unique_ptr<T>(nullptr);
		}

		template<class T>
		std::vector<std::unique_ptr<T>> deep_copy(const std::vector<std::unique_ptr<T>>& t_vec)
		{
			std::vector<std::unique_ptr<T>> v;
			for(const auto& x:t_vec)
			{
				v.push_back(deep_copy(x));
			}
			return v;
		}
	}
}
