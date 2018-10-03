// File: is_specialization_of.h
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

#include <type_traits>

namespace metl
{
	namespace internal
	{
		template<class, template<class...> class>
		struct is_specialization_of: std::false_type{};

		template<template<class...> class Templ, class... Ts>
		struct is_specialization_of<Templ<Ts...>, Templ> : std::true_type{};

		template<class T, template<class...> class Temp>
		constexpr static bool is_specialization_of_v = is_specialization_of<T, Temp>::value;
		
	}
}