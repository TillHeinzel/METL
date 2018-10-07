
#pragma once


#include <string>
#include <vector>
#include <cassert>

#include "TypeErasure/TypeEnum.hpp"

#include "src/UnaryID.hpp"

namespace metl
{
	namespace internal
	{
		struct UnarySignature
		{
			std::string name;
			TYPE argument;
		};

		inline UnarySignature makeSignature(const UnaryID& id, const std::vector<TYPE>& arguments)
		{
			assert(arguments.size() == 1);
			return {id.name, arguments.front()};
		}
	}
}
