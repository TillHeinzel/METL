
#pragma once


#include <string>
#include <vector>

#include "src/TypeErasure/TypeEnum.hpp"

#include "src/OperationIdentification/FunctionID.hpp"
#include "src/OperationIdentification/OperationSignature.hpp"

namespace metl
{
	namespace internal
	{
		using FunctionSignature = OperationSignature<FunctionID>;

		inline FunctionSignature makeSignature(const FunctionID& id, const std::vector<TYPE>& arguments)
		{
			return {id.name, arguments};
		}
	}
}
