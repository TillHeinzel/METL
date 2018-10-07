
#pragma once


#include <string>
#include <vector>

#include "TypeErasure/TypeEnum.hpp"

#include "src/FunctionID.hpp"

namespace metl
{
	namespace internal
	{
		struct FunctionSignature
		{
			std::string name;
			std::vector<TYPE> arguments;
		};

		inline FunctionSignature makeSignature(const FunctionID& id, const std::vector<TYPE>& arguments)
		{
			return {id.name, arguments};
		}
	}
}
