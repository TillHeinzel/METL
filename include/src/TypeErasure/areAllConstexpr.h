#pragma once

namespace metl
{
	namespace internal
	{
		template<class UntypedExpression_t>
		bool areAllConstexpr(const UntypedExpression_t& e)
		{
			return e.isConstexpr();
		}

		template<class UntypedExpression_t>
		bool areAllConstexpr(const std::vector<UntypedExpression_t>& v)
		{
			bool areAll = true;
			for(const auto &expr : v)
			{
				if(!expr.isConstexpr())
				{
					areAll = false;
					break;
				}
			}
			return areAll;
		}
	}
}