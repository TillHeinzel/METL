// File: apply_each.h
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

#include <cstddef>
#include <tuple>
#include <utility>

namespace metl
{
	namespace internal
	{
		namespace detail
		{
			template <typename Tuple, typename F, std::size_t ...Indices>
			constexpr void apply_each_impl(F&& f, Tuple&& tuple, std::index_sequence<Indices...>)
			{
				using swallow = int[];
				(void)swallow
				{
					1,
						(f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
				};
			}
		}

		template <typename Tuple, typename F>
		constexpr void apply_each(F&& f, Tuple&& tuple)
		{
			constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
			detail::apply_each_impl(std::forward<F>(f), std::forward<Tuple>(tuple),
						  std::make_index_sequence<N>{});
		}
	}
}
