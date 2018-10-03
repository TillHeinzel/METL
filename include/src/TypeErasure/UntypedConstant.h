// File: VariableWrapper.h
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

#include "ThirdParty/Variant/variant.hpp"

#include "src/TypeErasure/TypeEnum.h"
#include "src/TypeErasure/UntypedExpression.fwd.h"

namespace metl
{
	template<class... Ts>
	class UntypedConstant
	{
	public:
		template<class T>
		explicit UntypedConstant(T val);

		template<class T>
		void setValue(const T& t);

		UntypedExpression<Ts...> makeUntypedExpression() const;

		TYPE type() const;

	private:
		mpark::variant<Ts...> value_;
	};

}
