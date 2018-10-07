
#pragma once


#include <string>
#include <vector>
#include <cassert>

#include "TypeErasure/TypeEnum.hpp"

#include "src/SuffixID.hpp"

namespace metl
{
	namespace internal
	{
		struct SuffixSignature
		{
			std::string name;
			TYPE argument;
		};

		inline SuffixSignature makeSignature(const SuffixID& id, const std::vector<TYPE>& arguments)
		{
			assert(arguments.size() == 1);
			return {id.name, arguments.front()};
		}
	}
}
