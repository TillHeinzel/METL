/*
@file
Compiler.h
Public access-file for template class Compiler, containing the make-functions and helpers to construct a metl::Compiler

Copyright 2017 Till Heinzel

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include "Compiler.impl.h"
#include "sortAndAddDefaults.h"
#include "LiteralConverters.h"

namespace metl
{
	template<class... Ts, class IntConverter, class RealConverter>
	auto makeCompiler_impl(IntConverter intConv, RealConverter realConv)
	{
		auto literalsConverter = internal::makeLiteralConverters(intConv, realConv);
		return Compiler<internal::grammar, decltype(literalsConverter), Ts...>(literalsConverter);
	}


	template<class... SupportedTypes>
	auto makeDefaultSettings()
	{
		auto defaultIntConverter = makeDefaultIntConverter<SupportedTypes...>();
		auto defaultRealConverter = makeDefaultRealConverter<SupportedTypes...>();

		return internal::toTypeMap(defaultIntConverter, defaultRealConverter);
	}

	template<class... SupportedTypes, class... SettingIDs, class... Settings>
	auto makeCompiler(internal::SettingWrapper<SettingIDs, Settings>... settings) 
	{
		auto inputMap = internal::toTypeMap(settings...);
		auto defaultMap = makeDefaultSettings<SupportedTypes...>();

		auto makeCompiler_caller = [](auto... ts) {return makeCompiler_impl<SupportedTypes...>(ts...); };
		return internal::callWithOrderedAndDefaultedSettings(makeCompiler_caller, defaultMap, inputMap);
	}
}
