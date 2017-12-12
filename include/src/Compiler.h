#pragma once

#include "Compiler.impl.h"

namespace metl
{
	struct BadLiteralException: public std::runtime_error{explicit BadLiteralException(const std::string& s):runtime_error(s){} };

	namespace detail
	{
		template<class From_t, class To_t, class F>
		struct Converter
		{
			using From = From_t;
			using To = To_t;
			F f;
		};

		template<class IntConverter, class RealConverter>
		struct LiteralConverters
		{
			IntConverter toInt;
			RealConverter toReal;
		};

		template<class... Ts>
		auto makeLiteralConverters(Ts&&...ts) { return LiteralConverters<Ts...>{ts...}; }
	
		template<class T> struct DefaultConverterMaker{};
		template<> struct DefaultConverterMaker<int> { constexpr static auto make() { return [](const std::string& s) {return std::stoi(s); }; } };
		template<> struct DefaultConverterMaker<double> { constexpr static auto make() { return [](const std::string& s) {return std::stod(s); }; } };

		template<class T> constexpr char defaultConverterName[] = "bla";
		template<> constexpr char defaultConverterName<int> [3]  = "int";
		template<> constexpr char defaultConverterName<double> [4]  = "real";

		template<class T, class... Ts>
		auto getConverter()
		{
			return constexpr_if(std::integral_constant<bool, isInList<T, Ts...>()>(), [](auto _)
			{
				//return _([](const std::string& s) {return std::stoi(s); });
				return _(DefaultConverterMaker<T>::make());
			},
				[](auto _)
			{
				return _( [](const auto& s) -> Get_t<0, Ts...>
				{
					throw BadLiteralException(std::string("Literals of type ") + defaultConverterName<T> +" are not allowed!");
				});
			});
		}
		
		template<class... Ts, class TT, class... Converters>
		auto getConverter(TT, Converters... converters)
		{
			return getConverter<Ts...>(converters);
		}

		template<class T, class... Ts, class TT, class F, class... Converters>
		auto getConverter(Converter<T, TT, F> converter1, Converters...)
		{
			static_assert(isInList<TT>(), "type converted to must be part of compiler!");
			return converter1;
		}

		template<class... Ts, class... Converters>
		auto constructFullLiteralsConverter(Converters... converters)
		{
			const auto toInt = getConverter<int, Ts...>(converters...);
			//const auto toInt = [](const std::string& s) {return std::stoi(s); };
			const auto toReal = getConverter<double, Ts...>(converters...);

			return detail::makeLiteralConverters(toInt, toReal);
		}
	}

	template<class T, class F>
	detail::Converter<int, T, F> intConverter(F f) { return detail::Converter<int, T, F>{f}; }
	template<class T, class F>
	detail::Converter<double, T, F> realConverter(F f) { return detail::Converter<double, T, F>{f}; }


	template<class... Ts, class... Converters>
	auto makeCompiler(Converters... converters)
	{
		auto literalsConverter = detail::constructFullLiteralsConverter<Ts...>(converters...);
		return Compiler<grammar<realLiteral, intLiteral>, decltype(literalsConverter), Ts...>(literalsConverter);
	}
}
