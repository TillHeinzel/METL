#pragma once

#include <tao/pegtl.hpp>


namespace metl
{
	// Implementations of special rules are in bottom of file.

	namespace pegtl = tao::pegtl;
	using namespace pegtl;

	///////////////// ATOMICS ////////////////
	struct intLiteral
		: seq< plus<digit>> {};

	struct realLiteral
		: seq< plus<digit>, one<'.'>, plus<digit>> {};

	struct imagLiteral
		: seq< disable<sor<realLiteral, intLiteral>>, one<'i'> > {};

	struct Variable; // constants and variables are equivalent wrt the grammar. 


	struct Expr;
	struct bracket : seq< one< '(' >, Expr, one< ')' > > {};


	/////////////// FUNCTIONS ///////////

	struct FunctionName;

	struct FunctionStart: seq<FunctionName, at<one<'('>>>{};

	struct Function: if_must<FunctionStart, one<'('>, opt<list<Expr, one<','>, space>>, one<')'>>{};

	/////////////// RECURSIVE EXPRESSIONSTRUCTURE ///////////

	struct UnaryOperator;

	struct Atomic:
		seq<opt<UnaryOperator>, star<space>, sor<bracket, Function, Variable, imagLiteral, realLiteral, intLiteral>> {};
	
	struct Operator;
	struct Expr
		: list<Atomic, Operator, space> {};
	

	////////////////// GRAMMAR //////////////////////
	struct grammar
		: must<Expr, pegtl::eof> {};
}

namespace metl
{
	template<typename Input, class T>
	static auto match_any_recursive(Input& in, const std::map<std::string, T>& map, std::string t)
		-> typename std::map<std::string, T>::const_iterator
	{
		// t starts out empty, and slowly becomes the same as in.current, adding one char on each recursion-step. t is then searched in the keys of map.
		if (in.size() > t.size()) {
			t += in.peek_char(t.size()); // append another char to teststring t
			const auto i = map.lower_bound(t); // not sure why this works
			if (i != map.end()) {
				// recursion step. try the next one down the line with an additional char.
				// This happend before the termination, because we are expected to be greedy.
				auto i2 = match_any_recursive(in, map, t);
				if (i2 != map.end())
				{
					return i2;
				}
				if (i->first == t)  // recursion stop: if we found a match, return.
				{
					return i;
				}
			}
		}
		return map.end();
	}

	struct Variable // constants and variables are equivalent wrt the grammar
	{
		using analyze_t = analysis::generic< analysis::rule_type::ANY >;

		template< apply_mode A,
			rewind_mode M,
			template< typename... > class Action,
			template< typename... > class Control,
			typename Input, class Compiler, class... Others>
			static bool match(Input& in, Compiler& s, const Others&...)
		{
			const auto& map = s.getCandV();
			auto i = match_any_recursive(in, map, std::string());
			if (i != map.end())
			{
				if (A == pegtl::apply_mode::ACTION)
				{
					s.stack_.push(i->second);
				}
				in.bump(i->first.size());
				return true;
			}

			return false;
		}
	};

	struct Operator
	{
		using analyze_t = analysis::generic< analysis::rule_type::ANY >;

		template< apply_mode A,
			rewind_mode M,
			template< typename... > class Action,
			template< typename... > class Control,
			typename Input, class Compiler>
			static bool match(Input& in, Compiler& s)
		{
			// In here, we first check if we can match the input to an operator of the current precedence N.
			// Then, we check if it exists for the desired left and right types. 

			const auto& operators = s.getCarriers(); // maps the name of each operator to its precedence.
			auto it = match_any_recursive(in, operators, std::string{});

			if (it == operators.end()) return false; // no operator was found

			s.stack_.push(it->second);
			in.bump(it->first.size()); // remove the operator from the input.
			return true;
		}
	};

	struct UnaryOperator
	{
		using analyze_t = analysis::generic< analysis::rule_type::ANY >;

		template< apply_mode A,
			rewind_mode M,
			template< typename... > class Action,
			template< typename... > class Control,
			typename Input, class Compiler>
			static bool match(Input& in, Compiler& s)
		{
			// In here, we first check if we can match the input to an operator of the current precedence N.
			// Then, we check if it exists for the desired left and right types. 

			const auto& operators = s.getUnaryCarriers(); // maps the name of each operator to its precedence.
			auto it = match_any_recursive(in, operators, std::string{});

			if (it == operators.end()) return false; // no operator was found

			s.stack_.push(it->second);
			in.bump(it->first.size()); // remove the operator from the input.
			return true;
		}
	};

	struct FunctionName
	{
		using analyze_t = analysis::generic< analysis::rule_type::ANY >;

		template< apply_mode A,
			rewind_mode M,
			template< typename... > class Action,
			template< typename... > class Control,
			typename Input, class Compiler>
			static bool match(Input& in, Compiler& s)
		{
			const auto& functionNames = s.getFunctionNames();
			auto it = match_any_recursive(in, functionNames, std::string{});
			if (it == functionNames.end()) return false;

			in.bump(it->first.size()); // remove the operator from the input.
			return true;
		}
	};
}