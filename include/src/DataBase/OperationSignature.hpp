#pragma once

#include <string>
#include <vector>

#include "src/TypeErasure/TypeEnum.hpp"

namespace metl
{
	namespace internal
	{
		template<class Label>
		struct OperationSignature
		{
			std::string name;
			std::vector<TYPE> argumentTypes;
		};
	}
}
