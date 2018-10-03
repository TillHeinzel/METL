// File: callWithOrderedAndDefaultedSettings.h
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

#include "src/Utility/TypeList.h"
#include "src/Utility/TypeMap.h"
#include "src/Utility/ConstexprBranching.h"

#include "src/std17/bool_constant.h"

#include "src/std17/apply.h"

namespace metl
{
	namespace internal
	{
		template<class Key, class Map>
		constexpr auto contains(const Map&)
		{
			return std17::bool_constant<Map::template contains<Key>()>();
		}

		template<class CurrentKey, class MapOfDefaults, class MapOfInputs>
		auto getValuePreferInput(const MapOfDefaults& defaults, const MapOfInputs& inputs)
		{
			return internal::constexpr_ternary(contains<CurrentKey>(inputs),
				[&](auto _)
			{
				return get<CurrentKey>(_(inputs));
			},
				[&](auto _)
			{
				return get<CurrentKey>(_(defaults));
			});
		}

		template<class CheckPolicy, class DefaultsMap, class InputMap>
		auto sortAndAddDefaults_impl(const DefaultsMap defaults, const InputMap& inputs)
		{
			return std::make_tuple(getValuePreferInput<CheckPolicy>(defaults, inputs));
		}

		template<class... OrderedKeys, class DefaultsMap, class InputMap>
		auto sortAndAddDefaults_impl(const DefaultsMap defaults, const InputMap& inputs)
		{
			return std::make_tuple(getValuePreferInput<OrderedKeys>(defaults, inputs)...);
		}

		template<class... DefaultIDs, class... DefaultPolicies, class... InputIDs, class... InputPolicies >
		auto sortAndAddDefaults(const TypeMap<std::tuple<DefaultIDs...>, std::tuple<DefaultPolicies...>>& map, TypeMap<std::tuple<InputIDs...>, std::tuple<InputPolicies...>> inputs)
		{
			return sortAndAddDefaults_impl<DefaultIDs...>(map, inputs);
		}
	}
}

namespace metl
{
	namespace internal
	{

		template<class... DefaultIDs>
		constexpr bool areInputIDsValid(TypeList<DefaultIDs...>, TypeList<>)
		{
			return true;
		}

		template<class... DefaultIDs, class InputID, class... InputIDs >
		constexpr bool areInputIDsValid(TypeList<DefaultIDs...> d, TypeList<InputID, InputIDs...>)
		{
			return isInList<InputID, DefaultIDs...>() ? internal::areInputIDsValid(d, TypeList<InputIDs...>{}) : false;
		}

		template<class ID, class Setting>
		struct SettingWrapper
		{
			Setting v;
		};

		template<class ID, class Setting>
		SettingWrapper<ID, Setting> wrapSetting(Setting setting)
		{
			return { setting };
		}

		template<class... IDs, class... Settings>
		TypeMap<std::tuple<IDs...>, std::tuple<Settings...>> toTypeMap(SettingWrapper<IDs, Settings>... settings)
		{
			return makeTypeMap(TypeList<IDs...>(), std::make_tuple(settings.v...));
		}

		template<class F, class M, class InputMap>
		auto callWithOrderedAndDefaultedSettings(F f, M orderedDefaultMap, InputMap inputMap)
		{
			//static_assert(internal::areInputIDsValid(internal::getKeys(internal::Type<M>()), internal::getKeys(internal::Type<InputMap>())), "One of the passed-in settings is not allowed for this function");
			return std17::apply(f, internal::sortAndAddDefaults(orderedDefaultMap, inputMap));
		}
	}
}
