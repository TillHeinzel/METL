#pragma once
#include "Stack.fwd.h"

#include <cassert>

namespace metl
{
	namespace
	{
		template<class Expression>
		Expression evaluateConstExpr_impl(TypeList<>, const Expression&)
		{
			throw std::runtime_error("can't evaluate constexpr, because type is not in typelist. However that happened...");
		}

		template<class T, class... Ts, class Expression>
		Expression evaluateConstExpr_impl(TypeList<T, Ts...>, const Expression& expression)
		{
			return expression.type() == Expression::template toType<T>() ?
				makeConstExpression<Expression>(expression.template get<T>()()) :
				evaluateConstExpr_impl(TypeList<Ts...>{}, expression);
		}

		template<class... Ts>
		Expression2<Ts...> evaluateConstExpr(const Expression2<Ts...>& expression)
		{
			return evaluateConstExpr_impl(TypeList<Ts...>{}, expression);
		}
	}

	namespace
	{
		template<class Expression>
		std::vector<TYPE> getTypes(const std::vector<Expression>& expressions)
		{
			std::vector<TYPE> types{};
			for (const auto& expr : expressions)
			{
				types.push_back(expr.type());
			}
			return types;
		}

		inline std::vector<TYPE> addThis(std::vector<TYPE> left, TYPE right)
		{
			left.push_back(right);
			return left;
		}

		inline std::vector<std::vector<TYPE>> tensorSum(const std::vector<std::vector<TYPE>>& left, const std::vector<TYPE>& right)
		{
			std::vector<std::vector<TYPE>> retval{};
			for (const auto& x : left)
			{
				for (const auto& y : right)
				{
					retval.push_back(addThis(x, y));
				}
			}

			return retval;
		}
	}
}

namespace metl
{
	namespace
	{
		template<class Expression>
		Expression changeSign_impl(TypeList<>, const Expression&)
		{
			throw std::runtime_error("can't change sign, because type is not in typelist. However that happened...");
		}

		template<class T, class... Ts, class Expression>
		Expression changeSign_impl(TypeList<T, Ts...>, const Expression& expression)
		{
			return expression.type() == Expression::template toType<T>() ?
				Expression(exprType<T>([f{ expression.template get<T>() }](){return -f(); })) :
				changeSign_impl(TypeList<Ts...>{}, expression);
		}

		template<class... Ts>
		Expression2<Ts...> changeSign(const Expression2<Ts...>& expression)
		{
			return changeSign_impl(TypeList<Ts...>{}, expression);
		}
	}
}

namespace metl
{


	template <class ... Ts>
	Stack<Ts...>::Substack::Substack(
		const std::map<std::string, FunctionImpl<Expression>>& opMap,
		const std::map<std::string, FunctionImpl<Expression>>& functionMap,
		const std::map<std::string, CastImpl<Expression>>& castImplementations,
		const std::map<std::string, CastImpl<Expression>>& suffixImplementations,
		const std::map<TYPE, std::vector<TYPE>>& castDeclarations)
		:opMap_(opMap),
		functionMap_(functionMap),
		suffixMap_(suffixImplementations),
		castImplementations_(castImplementations),
		castDeclarations_(castDeclarations)
	{
	}

	template <class ... Ts>
	void Stack<Ts...>::Substack::push(Expression l)
	{
		expressions_.push_back(l);
	}

	template <class ... Ts>
	void Stack<Ts...>::Substack::push(const opCarrier& b)
	{
		if (b.associativity == ASSOCIATIVITY::LEFT)
		{
			while (operators_.size() > 0 && operators_.back().precedence <= b.precedence)
			{
				reduce();
			}
		}
		operators_.push_back(b);
	}

	template <class ... Ts>
	void Stack<Ts...>::Substack::pushFunction(std::string FunctionName)
	{
		function_ = std::make_unique<std::string>(FunctionName);
	}

	template <class ... Ts>
	void Stack<Ts...>::Substack::push(const suffixCarrier& suffix)
	{
		assert(!expressions_.empty());
		assert(expressions_.back().category() == CATEGORY::CONSTEXPR);

		const auto inType = expressions_.back().type();
		auto it = suffixMap_.find(mangleSuffix(suffix.name, { inType }));

		if (it == suffixMap_.end())
		{
			std::vector<TYPE> allowedCasts = castDeclarations_.at(inType);


			std::vector<TYPE> validCasts;
			std::vector<std::string> possibleFunctions;
			for (auto c : allowedCasts)
			{
				auto it2 = suffixMap_.find(mangleSuffix(suffix.name, { c }));
				if (it2 != suffixMap_.end())
				{
					possibleFunctions.push_back(it2->first);
					validCasts.push_back(c);
				}
			}
			// if we found multiple possible overloads that we can get through casts, this is an error
			if (possibleFunctions.size() > 1)
			{
				throw std::runtime_error("To many possible overloads for unary operator " + suffix.name);
			}

			// if we found a single possible overload that can be achieved through casting, use it!
			if (possibleFunctions.size() == 1)
			{
				castTo({ validCasts.back() });
				push(suffix); // call recursively
				return;
			}
			else
			{
				throw std::runtime_error("could not find a matching unary operator for " + suffix.name);
			}
		}

		// get left and right expressions
		const auto t = expressions_.back();
		expressions_.pop_back();
		
		auto resultExpression = it->second({ t });

		if (t.category() == CATEGORY::CONSTEXPR)
		{
			resultExpression = evaluateConstExpr(resultExpression);
		}

		expressions_.push_back(resultExpression);
	}

	template <class ... Ts>
	typename Stack<Ts...>::Expression Stack<Ts...>::Substack::finish()
	{
		if (function_)
		{
			evaluateFunction();
		}
		else
		{
			while (!operators_.empty())
			{
				reduce();
			}
		}
		assert(expressions_.size() == 1);

		const auto r = expressions_.back();
		expressions_.clear();
		return r;
	}

	template <class ... Ts>
	void Stack<Ts...>::Substack::evaluateFunction()
	{
		assert(operators_.empty());

		auto inTypes = getTypes(expressions_);

		auto it = functionMap_.find(mangleName(*function_, inTypes));
		if (it == functionMap_.end())
		{
			std::vector<std::vector<TYPE>> castCombis{ {} };
			for (auto t : inTypes)
			{
				castCombis = tensorSum(castCombis, castDeclarations_.at(t));
			}

			std::vector<std::vector<TYPE>> validCasts;
			std::vector<std::string> possibleFunctions;
			for (auto c : castCombis)
			{
				auto it2 = functionMap_.find(mangleName(*function_, c));
				if (it2 != functionMap_.end())
				{
					possibleFunctions.push_back(it2->first);
					validCasts.push_back(c);
				}
			}
			// if we found multiple possible overloads that we can get through casts, this is an error
			if (possibleFunctions.size() > 1)
			{
				throw std::runtime_error("To many possible overloads for function " + *function_);
			}

			// if we found a single possible overload that can be achieved through casting, use it!
			if (possibleFunctions.size() == 1)
			{
				castTo(validCasts.back());
				evaluateFunction(); // call recursively
				return;
			}
			else
			{
				throw std::runtime_error("could not find a matching function for " + *function_);
			}
		}

		auto resultExpression = it->second(expressions_);
		if (are_all_const(expressions_))
		{
			resultExpression = evaluateConstExpr(resultExpression);
		}

		expressions_.clear();
		expressions_.push_back(resultExpression);
	}

	template <class ... Ts>
	void Stack<Ts...>::Substack::reduce()
	{
		assert(!operators_.empty());
		if (operators_.back().isUnary) reduceUnary();
		else reduceBinary();
	}

	template <class ... Ts>
	void Stack<Ts...>::Substack::reduceBinary()
	{
		assert(expressions_.size() > 1);

		// get operator
		const auto opName = operators_.back().name;

		auto inTypes = std::vector<TYPE>{ (expressions_.rbegin() + 1)->type(), expressions_.rbegin()->type() };
		auto it = opMap_.find(mangleName(opName, inTypes));
		if (it == opMap_.end())
		{
			std::vector<std::vector<TYPE>> castCombis{ {} };
			for (auto t : inTypes)
			{
				castCombis = tensorSum(castCombis, castDeclarations_.at(t));
			}

			std::vector<std::vector<TYPE>> validCasts;
			std::vector<std::string> possibleFunctions;
			for (auto c : castCombis)
			{
				auto it2 = opMap_.find(mangleName(opName, c));
				if (it2 != opMap_.end())
				{
					possibleFunctions.push_back(it2->first);
					validCasts.push_back(c);
				}
			}
			// if we found multiple possible overloads that we can get through casts, this is an error
			if (possibleFunctions.size() > 1)
			{
				throw std::runtime_error("To many possible overloads for operator " + opName);
			}

			// if we found a single possible overload that can be achieved through casting, use it!
			if (possibleFunctions.size() == 1)
			{
				castTo(validCasts.back());
				reduce(); // call recursively
				return;
			}
			else
			{
				throw std::runtime_error("could not find a matching operator for " + opName);
			}
		}

		// get left and right expressions
		const auto r = expressions_.back();
		expressions_.pop_back();
		const auto l = expressions_.back();
		expressions_.pop_back();

		operators_.pop_back();

		auto resultExpression = it->second({ l, r });

		if (l.category() == CATEGORY::CONSTEXPR && r.category() == CATEGORY::CONSTEXPR)
		{
			resultExpression = evaluateConstExpr(resultExpression);
		}

		expressions_.push_back(resultExpression);
	}

	template <class ... Ts>
	void Stack<Ts...>::Substack::reduceUnary()
	{
		assert(expressions_.size() > 0u);

		// get operator
		const auto opName = operators_.back().name;

		auto inType = expressions_.back().type();
		auto it = opMap_.find(mangleName(opName, { inType }));
		if (it == opMap_.end())
		{
			std::vector<TYPE> allowedCasts = castDeclarations_.at(inType);


			std::vector<TYPE> validCasts;
			std::vector<std::string> possibleFunctions;
			for (auto c : allowedCasts)
			{
				auto it2 = opMap_.find(mangleName(opName, { c }));
				if (it2 != opMap_.end())
				{
					possibleFunctions.push_back(it2->first);
					validCasts.push_back(c);
				}
			}
			// if we found multiple possible overloads that we can get through casts, this is an error
			if (possibleFunctions.size() > 1)
			{
				throw std::runtime_error("To many possible overloads for unary operator " + opName);
			}

			// if we found a single possible overload that can be achieved through casting, use it!
			if (possibleFunctions.size() == 1)
			{
				castTo({ validCasts.back() });
				reduce(); // call recursively
				return;
			}
			else
			{
				throw std::runtime_error("could not find a matching unary operator for " + opName);
			}
		}

		// get left and right expressions
		const auto t = expressions_.back();
		expressions_.pop_back();

		operators_.pop_back();

		auto resultExpression = it->second({ t });

		if (t.category() == CATEGORY::CONSTEXPR)
		{
			resultExpression = evaluateConstExpr(resultExpression);
		}

		expressions_.push_back(resultExpression);
	}

	template <class ... Ts>
	bool Stack<Ts...>::Substack::are_all_const(const std::vector<Expression>& expressions)
	{
		for (const auto& e : expressions)
		{
			if (e.category() != CATEGORY::CONSTEXPR) return false;
		}
		return true;
	}

	template <class ... Ts>
	void Stack<Ts...>::Substack::castTo(const std::vector<TYPE>& targetTypes)
	{
		auto i_target = 0u;
		// we cast only the last size(targetTypes) expressions. This then works for both functions and operators
		for (auto i = expressions_.size() - targetTypes.size(); i < expressions_.size(); ++i)
		{
			auto& expr = expressions_.at(i);
			const auto fromType = expr.type();
			const auto toType = targetTypes.at(i_target);
			if (fromType != toType)
			{
				if (expr.category() == CATEGORY::CONSTEXPR)
				{
					expr = evaluateConstExpr(castImplementations_.at(mangleCast(expr.type(), targetTypes.at(i_target)))(expr));
				}
				else
				{
					expr = castImplementations_.at(mangleCast(expr.type(), targetTypes.at(i)))(expr);
				}
			}
			++i_target;
		}
	}
}

namespace metl
{
	template <class ... Ts>
	Stack<Ts...>::Stack(
		const std::map<std::string, FunctionImpl<Expression>>& opMap, 
		const std::map<std::string, FunctionImpl<Expression>>& funcMap, 
		const std::map<std::string, CastImpl<Expression>>& castImplementations, 
		const std::map<std::string, CastImpl<Expression>>& suffixImplementations, 
		const std::map<TYPE, std::vector<TYPE>>& castDeclarations)
		:opMap_(opMap),
		funcMap_(funcMap),
		castImplementations_(castImplementations),
		suffixImplementations_(suffixImplementations),
		castDeclarations_(castDeclarations)
	{
		open();
	}
	template <class ... Ts>
	void Stack<Ts...>::push(const Expression& t)
	{
		assert(!subStacks_.empty());
		subStacks_.back().push(t);
	}

	template <class ... Ts>
	void Stack<Ts...>::pushFunction(std::string functionName)
	{
		open();
		subStacks_.back().pushFunction(functionName);
	}

	template <class ... Ts>
	void Stack<Ts...>::push(const opCarrier& op)
	{
		subStacks_.back().push(op);
	}

	template <class ... Ts>
	void Stack<Ts...>::push(const suffixCarrier& suffix)
	{
		subStacks_.back().push(suffix);
	}

	template <class ... Ts>
	void Stack<Ts...>::open()
	{
		subStacks_.emplace_back(opMap_, funcMap_, castImplementations_, suffixImplementations_, castDeclarations_);
	}

	template <class ... Ts>
	void Stack<Ts...>::close()
	{
		assert(subStacks_.size() > 1);
		if (subStacks_.back().empty()) // special case for function taking zero arguments.
		{
			subStacks_.pop_back();
		}
		else
		{
			const auto r = subStacks_.back().finish();
			subStacks_.pop_back();
			subStacks_.back().push(r);
		}
	}

	template <class ... Ts>
	typename Stack<Ts...>::Expression Stack<Ts...>::finish()
	{
		assert(subStacks_.size() == 1);
		return subStacks_.back().finish();
	}

	template <class ... Ts>
	void Stack<Ts...>::clear()
	{
		subStacks_.clear();
		open();
	}
}
