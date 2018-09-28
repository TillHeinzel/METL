/*
@file ExpressionType.h

Copyright 2017-2018 Till Heinzel

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

#include <functional>

namespace metl
{
	template<class T> using TypedExpression = std::function<T()>;
}

//#include <type_traits>
//
//namespace metl
//{
//	template<class T>
//	class TypedExpression
//	{
//		using FunctionType = std::function<T()>;
//	public:
//
//		template<class F, typename = std::enable_if_t<!std::is_constructible<FunctionType, const F&>::value>>
//		TypedExpression(const F& f, ...)
//		{}
//
//		template<class F, typename = std::enable_if_t<std::is_constructible<FunctionType, const F&>::value>>
//		TypedExpression(const F& f) :
//			f_(f)
//		{}
//
//		T operator() () const
//		{
//			return f_();
//		}
//
//		template<class... TT>
//		auto* target()
//		{
//			return f_.target<TT...>();
//		}
//	private:
//		FunctionType f_;
//	};
//}
//
