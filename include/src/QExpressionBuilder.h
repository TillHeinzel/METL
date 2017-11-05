#pragma once

#include "ExpressionBuilder.h"

namespace expressionbuilder
{
	using qengine::real;
	using qengine::complex;
	using qengine::RVec;
	using qengine::CVec;

	using QExpressionBuilder = ExpressionBuilder<int, real, complex, RVec, CVec>;

	void addDefaults(QExpressionBuilder& builder);
}
