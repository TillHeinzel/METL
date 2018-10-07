
#pragma once

#include <cassert>

#include "src/OperationIdentification/BinaryID.hpp"
#include "src/OperationIdentification/OperationSignature.hpp"

namespace metl
{
	namespace internal
	{
		using BinarySignature = OperationSignature<BinaryID>;

		inline BinarySignature makeSignature(const BinaryID& id, const std::vector<TYPE>& arguments)
		{
			assert(arguments.size() == 2);
			return {id.name, arguments};
		}
	}
}
