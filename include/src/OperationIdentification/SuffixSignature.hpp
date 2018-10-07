
#pragma once


#include <string>
#include <vector>
#include <cassert>

#include "src/TypeErasure/TypeEnum.hpp"

#include "src/OperationIdentification/SuffixID.hpp"
#include "src/OperationIdentification/OperationSignature.hpp"

namespace metl
{
	namespace internal
	{
		using SuffixSignature = OperationSignature<SuffixID>;

		inline SuffixSignature makeSignature(const SuffixID& id, const std::vector<TYPE>& arguments)
		{
			assert(arguments.size() == 1);
			return {id.name, arguments};
		}
	}
}
