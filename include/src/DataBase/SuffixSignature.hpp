
#pragma once


#include <string>
#include <vector>
#include <cassert>

#include "src/TypeErasure/TypeEnum.hpp"

#include "src/DataBase/SuffixID.hpp"
#include "src/DataBase/OperationSignature.hpp"

namespace metl
{
	namespace internal
	{
		using SuffixSignature = OperationSignature<SuffixLabel>;

		inline SuffixSignature makeSignature(const SuffixID& id, const std::vector<TYPE>& arguments)
		{
			assert(arguments.size() == 1);
			return {id.name, arguments};
		}
	}
}
