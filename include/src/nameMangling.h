#pragma once
#include <vector>
#include <functional>
#include <string>

namespace metl
{
	enum class TYPE : int;

	inline std::string typeToString(TYPE type)
	{
		return std::to_string(int(type));
	}

	inline std::vector<std::string> typeToString(const std::vector<TYPE>& types)
	{
		std::vector<std::string> ret{};
		for(const auto& type:types)
		{
			ret.push_back(typeToString(type));
		}
		return ret;
	}
}

namespace metl
{
	constexpr char separator = '@';

	inline std::string mangleCast(TYPE from, TYPE to)
	{
		return typeToString(from) + separator + typeToString(to);
	}

	inline std::string mangleName(std::string functionName, const std::vector<TYPE>& paramTypes)
	{
		for (const auto& t : paramTypes)
		{
			functionName += separator + typeToString(t);
		}

		return functionName;
	}

	template<class Expr>
	std::string mangleName(std::string functionName, const std::vector<Expr>& params)
	{
		for (const auto& t : params)
		{
			functionName += separator + typeToString(t.type());
		}

		return functionName;
	}
}