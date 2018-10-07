// File: areAllConstexpr.h
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

namespace metl
{
	namespace internal
	{
		template<class UntypedExpression_t>
		bool areAllConstexpr(const UntypedExpression_t& e)
		{
			return e.isConstexpr();
		}

		template<class UntypedExpression_t>
		bool areAllConstexpr(const std::vector<UntypedExpression_t>& v)
		{
			for(const auto &expr : v)
			{
				if(!expr.isConstexpr())
				{
					return false;
				}
			}
			return true;
		}
	}
}
