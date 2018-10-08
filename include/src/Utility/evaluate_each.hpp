// File: evaluate_each.h
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

#include <tuple>

namespace metl
{
	namespace internal
	{
		namespace detail
		{


			template<class F, class Tuple, std::size_t... I>
			decltype(auto) evaluate_each_impl(F&& f, Tuple&& tuple, std::index_sequence<I...>)
			{
				using RetTuple = std::tuple<decltype(std::forward<F>(f)(std::get<I>(std::forward<Tuple>(tuple))))...>;

				return RetTuple(std::make_tuple(std::forward<F>(f)(std::get<I>(std::forward<Tuple>(tuple)))...));
			}
		}

		template<class F, class Tuple>
		decltype(auto) evaluate_each(F&& f, Tuple&& tuple)
		{
			return detail::evaluate_each_impl(std::forward<F>(f), std::forward<Tuple>(tuple),
										   std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
		}



		template<class Tuple>
		decltype(auto) evaluate_each(Tuple&& callables)
		{
			return internal::evaluate_each([](const auto& f) {return f(); }, std::forward<Tuple>(callables));
		}
	}
}
