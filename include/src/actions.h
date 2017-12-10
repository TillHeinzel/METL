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
			compiler.stack_.push(metl::makeConstExpression<typename Compiler::Expression>(compiler.literalConverters_.toInt(in.string())));
		}
	};

	template<>
	struct action<realLiteral>
	{
		template< typename Input, class Compiler >
		static void apply(const Input& in, Compiler& compiler)
		{
			compiler.stack_.push(metl::makeConstExpression<typename Compiler::Expression>(compiler.literalConverters_.toReal(in.string())));
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
			compiler.stack_.pushFunction(in.string());
		}
	};	
	
	template<>
	struct action<one<'('>>
	{
		template< typename Input, class Compiler >
		static void apply(const Input&, Compiler& compiler)
		{
			compiler.stack_.open();
		}
	};	

	template<>
	struct action<one<')'>>
	{
		template< typename Input, class Compiler >
		static void apply(const Input&, Compiler& compiler)
		{
			compiler.stack_.close();
		}
	};
	template<>
	struct action<one<','>>
	{
		template< typename Input, class Compiler >
		static void apply(const Input&, Compiler& compiler)
		{
			compiler.stack_.close();
			compiler.stack_.open();
		}
	};

	template<class... Literals>
	struct action<Function<Literals...>>
	{
		template< typename Input, class Compiler >
		static void apply(const Input&, Compiler& compiler)
		{
			compiler.stack_.close();
		}
	};
}
