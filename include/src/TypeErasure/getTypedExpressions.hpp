// File: getTypedExpressions.h
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
		namespace detail
		{
			template <class... Ts, std::size_t... Ind, class Expression>
			auto getTypedExpressions_impl(const std::vector<Expression>& v, std::index_sequence<Ind...>)
			{
				return std::make_tuple(v.at(Ind).template get<Ts>()...);
			}
		}

		template <class... Ts, class Expression>
		auto getTypedExpressions(const std::vector<Expression>& v)
		{
			return detail::getTypedExpressions_impl<Ts...>(v, std::make_index_sequence<sizeof...(Ts)>{});
		}

		template <class T, class Expression>
		auto getTypedExpressions(const Expression& e)
		{
			return std::make_tuple(e.template get<T>());
		}
	}
}
