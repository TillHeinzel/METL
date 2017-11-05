#pragma once

#include "Compiler.impl.h"

namespace metl
{
	template<class... Ts>
	Compiler<int, double, Ts...> makeCompiler(const Ts&...)
	{
		return Compiler<int, double, Ts...>{};
	}

	template<class T> struct IntType{};
	template<class T> struct FloatType{};

	template<class Int_T, class... Ts>
	Compiler<Int_T, double, Ts...> makeCompiler(const IntType<Int_T>&, const Ts&...)
	{
		return Compiler<Int_T, double, Ts...>{};
	}

	template<class Float_T, class... Ts>
	Compiler<int, Float_T, Ts...> makeCompiler(const FloatType<Float_T>&, const Ts&...)
	{
		return Compiler<int, Float_T, Ts...>{};
	}

	template<class Int_T, class Float_T, class... Ts>
	Compiler<Int_T, Float_T, Ts...> makeCompiler(const IntType<Int_T>&, const FloatType<Float_T>&, const Ts&...)
	{
		return Compiler<Int_T, Float_T, Ts...>{};
	}
}