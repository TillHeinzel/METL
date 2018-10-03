// File: StackHelpers.h
// 
// Copyright 2017-2018 Till Heinzel
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <vector>

#include "src/TypeErasure/TypeEnum.h"

namespace metl
{
	namespace internal
	{
		template<class Expression>
		std::vector<TYPE> getTypes(const std::vector<Expression>& expressions)
		{
			std::vector<TYPE> types{};
			for(const auto& expr : expressions)
			{
				types.push_back(expr.type());
			}
			return types;
		}

		inline std::vector<TYPE> addThis(std::vector<TYPE> left, TYPE right)
		{
			left.push_back(right);
			return left;
		}

		inline std::vector<std::vector<TYPE>> tensorSum(const std::vector<std::vector<TYPE>>& left, const std::vector<TYPE>& right)
		{
			std::vector<std::vector<TYPE>> retval{};
			for(const auto& x : left)
			{
				for(const auto& y : right)
				{
					retval.push_back(addThis(x, y));
				}
			}

			return retval;
		}
	}
}
