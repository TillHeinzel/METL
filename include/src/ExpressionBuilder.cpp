
// defined before including ExpressionBuilder.impl.h
// N is the precedence of the currently considered operators. This is harcoded here to be max 6, because we currently need way less.

#include "src/Types.h"
#include "src/Constants.h"
#include "src/linear_algebra/Vector.h"

#include "ExpressionBuilder.impl.h"

namespace expressionbuilder
{
	using qengine::real;
	using qengine::complex;
	using qengine::RVec;
	using qengine::CVec;

	template class ExpressionBuilder<int, real, complex, RVec, CVec>;
}
