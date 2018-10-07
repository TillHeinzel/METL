
#pragma once

#include <cassert>

#include "src/DataBase/BinaryID.hpp"
#include "src/DataBase/OperationSignature.hpp"

namespace metl
{
	namespace internal
	{
		using BinarySignature = OperationSignature<BinaryLabel>;

		inline BinarySignature makeSignature(const BinaryID& id, const std::vector<TYPE>& arguments)
		{
			assert(arguments.size() == 2);
			return {id.name, arguments};
		}
	}
}
