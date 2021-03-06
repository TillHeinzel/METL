

#pragma once


#include <string>
#include <vector>
#include <cassert>

#include "src/TypeErasure/TypeEnum.hpp"

#include "src/DataBase/UnaryID.hpp"
#include "src/DataBase/OperationSignature.hpp"

namespace metl
{
	namespace internal
	{
		using UnarySignature = OperationSignature<UnaryLabel>;

		inline UnarySignature makeSignature(const UnaryID& id, const std::vector<TYPE>& arguments)
		{
			assert(arguments.size() == 1);
			return {id.name, arguments};
		}
	}
}
