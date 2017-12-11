#pragma once
#include "grammar.h"

namespace metl
{
	template< typename Rule >
	struct action
		: nothing<Rule>
	{};

	template<>
	struct action<intLiteral>
	{
		template< typename Input, class Compiler >
		static void apply(const Input& in, Compiler& compiler)
		{
			compiler.impl_.stack_.push(metl::makeConstExpression<typename Compiler::Expression>(compiler.impl_.literalConverters_.toInt(in.string())));
		}
	};

	template<>
	struct action<realLiteral>
	{
		template< typename Input, class Compiler >
		static void apply(const Input& in, Compiler& compiler)
		{
			compiler.impl_.stack_.push(metl::makeConstExpression<typename Compiler::Expression>(compiler.impl_.literalConverters_.toReal(in.string())));
		}
	};

	//template<>
	//struct action<imagLiteral>
	//{
	//	template< typename Input, class Compiler >
	//	static void apply(const Input& in, Compiler& compiler)
	//	{
	//		double d = std::stod(in.string());
	//		compiler.stack_.push(metl::makeConstExpression<typename Compiler::Expression>(d*IMAGUNIT));
	//	}
	//};

	template<>
	struct action<FunctionStart>
	{
		template< typename Input, class Compiler >
		static void apply(const Input& in, Compiler& compiler)
		{
			compiler.impl_.stack_.pushFunction(in.string());
		}
	};	
	
	template<>
	struct action<one<'('>>
	{
		template< typename Input, class Compiler >
		static void apply(const Input&, Compiler& compiler)
		{
			compiler.impl_.stack_.open();
		}
	};	

	template<>
	struct action<one<')'>>
	{
		template< typename Input, class Compiler >
		static void apply(const Input&, Compiler& compiler)
		{
			compiler.impl_.stack_.close();
		}
	};
	template<>
	struct action<one<','>>
	{
		template< typename Input, class Compiler >
		static void apply(const Input&, Compiler& compiler)
		{
			compiler.impl_.stack_.close();
			compiler.impl_.stack_.open();
		}
	};

	template<class... Literals>
	struct action<Function<Literals...>>
	{
		template< typename Input, class Compiler >
		static void apply(const Input&, Compiler& compiler)
		{
			compiler.impl_.stack_.close();
		}
	};
}
