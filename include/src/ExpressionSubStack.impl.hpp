#pragma once

#include "src/ExpressionSubStack.h"

namespace metl
{
	namespace internal
	{

		template <class ... Ts>
		ExpressionSubStack<Ts...>::ExpressionSubStack(const CompilerEntityDataBase<Ts...>& dataBase) :
			operationApplier_(dataBase)
		{}

		template <class ... Ts>
		void ExpressionSubStack<Ts...>::push(Expression l)
		{
			subExpressions_.push_back(std::move(l));
		}

		template <class ... Ts>
		void ExpressionSubStack<Ts...>::push(const BasicOperatorData& b)
		{
			if(b.associativity == ASSOCIATIVITY::LEFT)
			{
				while(operators_.size() > 0 && operators_.back().precedence <= b.precedence)
				{
					reduce();
				}
			}
			operators_.push_back(b);
		}

		template <class ... Ts>
		void ExpressionSubStack<Ts...>::push(const SuffixID& suffix)
		{
			assert(!subExpressions_.empty());
			assert(subExpressions_.back().isConstexpr()); // can't explicitly check that it is a literal

			const auto t = subExpressions_.back();
			subExpressions_.pop_back();

			subExpressions_.push_back(operationApplier_.apply(suffix, {t}));
		}

		template <class ... Ts>
		typename ExpressionSubStack<Ts...>::Expression ExpressionSubStack<Ts...>::finish()
		{
			while(!operators_.empty())
			{
				reduce();
			}
			assert(subExpressions_.size() == 1);

			const auto r = subExpressions_.back();
			subExpressions_.clear();
			return r;
		}

		template <class ... Ts>
		bool ExpressionSubStack<Ts...>::empty() const
		{
			return subExpressions_.empty();
		}

		template <class ... Ts>
		void ExpressionSubStack<Ts...>::reduceUnary()
		{
			assert(subExpressions_.size() > 0u);

			// get operator
			const auto opName = operators_.back().name;
			operators_.pop_back();

			const auto t = subExpressions_.back();
			subExpressions_.pop_back();

			subExpressions_.push_back(operationApplier_.apply(UnaryID{opName}, {t}));
		}

		template <class ... Ts>
		void ExpressionSubStack<Ts...>::reduceBinary()
		{
			assert(subExpressions_.size() > 1);

			// get operator
			const auto opName = operators_.back().name;
			operators_.pop_back();

			const auto r = subExpressions_.back();
			subExpressions_.pop_back();
			const auto l = subExpressions_.back();
			subExpressions_.pop_back();

			subExpressions_.push_back(operationApplier_.apply(BinaryID{opName}, {l,r}));
		}
		
		template <class ... Ts>
		void ExpressionSubStack<Ts...>::reduce()
		{
			assert(!operators_.empty());
			if(operators_.back().isUnary) reduceUnary();
			else reduceBinary();
		}
	}
}