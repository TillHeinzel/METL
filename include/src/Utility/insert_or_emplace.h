// File: insert_or_emplace.h
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
#include <map>
#include <string>

namespace metl
{
	namespace internal
	{
		template<class Key, class T>
		void insert_or_emplace(std::map<Key, T>& map, const Key& key, const T& val)
		{
			auto it = map.find(key);
			if (it == map.end())
			{
				map.emplace(key, val);
				return;
			}
			it->second = val;
		}
	}
}
