
#pragma once


#include <string>
#include <vector>
#include <cassert>

#include "TypeErasure/TypeEnum.hpp"

#include "src/BinaryID.hpp"

namespace metl
{
	namespace internal
	{
		struct BinarySignature
		{
			std::string name;
			TYPE leftArgument;
			TYPE rightArgument;
		};

		inline BinarySignature makeSignature(const BinaryID& id, const std::vector<TYPE>& arguments)
		{
			assert(arguments.size() == 2);
			return {id.name, arguments.front(), arguments.back()};
		}
	}
}
