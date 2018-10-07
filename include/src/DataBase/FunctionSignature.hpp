
#pragma once


#include <string>
#include <vector>

#include "src/TypeErasure/TypeEnum.hpp"

#include "src/DataBase/FunctionID.hpp"
#include "src/DataBase/OperationSignature.hpp"

namespace metl
{
	namespace internal
	{
		using FunctionSignature = OperationSignature<FunctionLabel>;

		inline FunctionSignature makeSignature(const FunctionID& id, const std::vector<TYPE>& arguments)
		{
			return {id.name, arguments};
		}
	}
}
