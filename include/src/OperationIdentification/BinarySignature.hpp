
#pragma once


#include <string>
#include <vector>
#include <cassert>

#include "src/TypeErasure/TypeEnum.hpp"

#include "src/OperationIdentification/BinaryID.hpp"
#include "src/OperationIdentification/OperationSignature.hpp"

namespace metl
{
	namespace internal
	{
		template<>
		struct OperationSignature<BinaryID>
		{
			std::string name;
			TYPE leftArgument;
			TYPE rightArgument;
		};

		using BinarySignature = OperationSignature<BinaryID>;

		inline BinarySignature makeSignature(const BinaryID& id, const std::vector<TYPE>& arguments)
		{
			assert(arguments.size() == 2);
			return {id.name, arguments.front(), arguments.back()};
		}
	}
}
