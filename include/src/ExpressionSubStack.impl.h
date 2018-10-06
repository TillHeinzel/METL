#pragma once

#include "src/ExpressionSubStack.h"

#include "src/Utility/get_each.h"

namespace metl
{
	namespace internal
	{

		template <class ... Ts>
		ExpressionSubStack<Ts...>::ExpressionSubStack(const CompilerEntityDataBase<Ts...>& dataBase) :
			dataBase_(dataBase),
			caster_(dataBase)
		{}

		template <class ... Ts>
		void ExpressionSubStack<Ts...>::push(Expression l)
		{
			subExpressions_.push_back(std::move(l));
		}

		template <class ... Ts>
		void ExpressionSubStack<Ts...>::push(const opCarrier& b)
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
		void ExpressionSubStack<Ts...>::push(const suffixCarrier& suffix)
		{
			assert(!subExpressions_.empty());
			assert(subExpressions_.back().isConstexpr());

			const auto t = subExpressions_.back();
			subExpressions_.pop_back();

			subExpressions_.push_back(applySuffix(suffix.name, t));
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
		UntypedExpression<Ts...> ExpressionSubStack<Ts...>::applySuffix(const std::string& suffix, const Expression& argument) const
		{

			const auto inType = argument.type();
			auto suffixImplOpt = dataBase_.findSuffix(mangleSuffix(suffix, {inType}));

			if(!suffixImplOpt)
			{
				auto targetType = caster_.findTypeForSuffix(suffix, inType);
				auto castedArguments = caster_.castTo2({argument}, {targetType});
				return applySuffix(suffix, castedArguments.front());
			}

			return suffixImplOpt->apply({argument});
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

			subExpressions_.push_back(applyUnary(opName, t));
		}

		template <class ... Ts>
		UntypedExpression<Ts...> ExpressionSubStack<Ts...>::applyUnary(const std::string& opName,
																	   const Expression& argument) const
		{
			auto inType = argument.type();
			auto operatorImplOpt = dataBase_.findOperator(mangleName(opName, {inType}));

			if(!operatorImplOpt)
			{
				auto targetType = caster_.findTypeForUnaryOperator(opName, inType);
				auto castedArgument = caster_.castTo2({argument}, {targetType});
				return applyUnary(opName, castedArgument.front()); // call recursively
			}

			return operatorImplOpt->apply({argument});
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

			subExpressions_.push_back(applyBinary(opName, {l,r}));
		}

		template <class ... Ts>
		UntypedExpression<Ts...> ExpressionSubStack<Ts...>::applyBinary(const std::string& opName, const std::vector<Expression>& arguments) const
		{

			auto inTypes = get_each(arguments, [](const auto& expr)
			{
				return expr.type();
			});
			auto operatorImplOpt = dataBase_.findOperator(mangleName(opName, inTypes));

			if(!operatorImplOpt)
			{
				auto targetTypes = caster_.findTypesForBinaryOperator(opName, inTypes);
				auto castedArguments = caster_.castTo2(arguments, targetTypes);
				return applyBinary(opName, castedArguments); // call recursively
			}

			return operatorImplOpt->apply(arguments);
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