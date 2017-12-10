#pragma once

#include "Compiler.impl.h"

namespace metl
{
	namespace detail
	{
		template<class IntConverter, class RealConverter>
		struct LiteralConverters
		{
			IntConverter toInt;
			RealConverter toReal;
		};

		template<class... Ts>
		auto makeLiteralConverters(Ts&&...ts) { return LiteralConverters<Ts...>{ts...}; }


	}


	template<class... Ts>
	auto makeCompiler()
	{
		static_assert(std::is_same_v<Get_t<0, Ts...>, int> && std::is_same_v<Get_t<1, Ts...>, double> , "The parameters must be {int, double, others...}");

		auto toInt = [](const std::string& s) {return std::stoi(s); };
		auto toReal = [](const std::string& s) {return std::stod(s); };

		auto literalsConverter = metl::detail::makeLiteralConverters(toInt, toReal);
		return Compiler<grammar<realLiteral, intLiteral>, decltype(literalsConverter), Ts...>(literalsConverter);
	}

	/*
	template<class... Ts>
	Compiler<int, double, Ts...> makeCompiler(const Ts&...)
	{
		return Compiler<grammar<intLiteral, realLiteral>, int, double, Ts...>{};
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
	}*/
}