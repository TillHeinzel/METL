
#include "QExpressionBuilder.h"

namespace expressionbuilder
{
	template<class T, class Builder, class F>
	void add_binaryOperators(Builder&, std::string, const F&)
	{
	}

	// adds all combinations of binary operators having T as the left side and T2, Ts... as the right side.
	// so add_binaryOperators<T, T2, T3, T4>(b, "+", [](auto a, auto b){return a+b;}) creates plus-operators for T+T2, T+T3 and T+T4
	template<class T, class T2, class... Ts, class Builder, class F>
	void add_binaryOperators(Builder& builder, std::string token, const F& f)
	{
		builder.template setOperator<T, T2>(token, f);
		add_binaryOperators<T, Ts...>(builder, token, f);
	}


	template<class Builder, class F>
	void add_unaryOperators(Builder&, std::string, const F&)
	{
	}

	template<class T, class... Ts, class Builder, class F>
	void add_unaryOperators(Builder& builder, std::string token, const F& f)
	{
		builder.template setUnaryOperator<T>(token, f);
		add_unaryOperators<Ts...>(builder, token, f);
	}
}

namespace expressionbuilder
{
	void addDefaults(QExpressionBuilder& builder)
	{
		using namespace qengine;
		using Expression = QExpressionBuilder::Expression;

		builder.setCast<int>([](int a) { return static_cast<double>(a); });

		//// Default constants
		builder.setConstant("pi", PI);
		builder.setConstant("i", IMAGUNIT);
		builder.setConstant("e", std::exp(1));

		////operator precedence
		builder.setOperatorPrecedence("+", 3);
		builder.setOperatorPrecedence("-", 3);
		builder.setOperatorPrecedence("*", 2);
		builder.setOperatorPrecedence("/", 2);
		builder.setUnaryOperatorPrecedence("-", 1);
		builder.setUnaryOperatorPrecedence("+", 1);
		builder.setOperatorPrecedence("^", 0);

		//// unary operators
		auto fUnaryPlus = [](auto a) { return a; };
		auto fUnaryMinus = [](auto a) { return -a; };

		add_unaryOperators<int, real, complex, RVec, CVec>(builder, "+", fUnaryPlus);
		add_unaryOperators<int, real, complex, RVec, CVec>(builder, "-", fUnaryMinus);

		//// Arithmetic
		// add arithmetic operators for each combination of input parameters. add_next<Operator, T, Ts...>(v) adds the Operator for all combinations of T on the left and Ts... on the right.

		auto fplus = [](auto left, auto right) { return left + right; };
		auto fminus = [](auto left, auto right) { return left - right; };
		auto fmul = [](auto left, auto right) { return left * right; };
		auto fpwmul = [](auto left, auto right) { return left % right; };
		auto fdiv = [](auto left, auto right) { return left / right; };

		add_binaryOperators<int, int>(builder, "+", fplus);
		add_binaryOperators<real, real, complex, RVec, CVec>(builder, "+", fplus);
		add_binaryOperators<complex, real, complex, RVec, CVec>(builder, "+", fplus);
		add_binaryOperators<RVec, real, complex, RVec, CVec>(builder, "+", fplus);
		add_binaryOperators<CVec, real, complex, RVec, CVec>(builder, "+", fplus);

		add_binaryOperators<int, int>(builder, "-", fminus);
		add_binaryOperators<real, real, complex, RVec, CVec>(builder, "-", fminus);
		add_binaryOperators<complex, real, complex, RVec, CVec>(builder, "-", fminus);
		add_binaryOperators<RVec, real, complex, RVec, CVec>(builder, "-", fminus);
		add_binaryOperators<CVec, real, complex, RVec, CVec>(builder, "-", fminus);

		add_binaryOperators<int, int>(builder, "*", fmul);
		add_binaryOperators<real, real, complex, RVec, CVec>(builder, "*", fmul);
		add_binaryOperators<complex, real, complex, RVec, CVec>(builder, "*", fmul);
		add_binaryOperators<RVec, real, complex>(builder, "*", fmul);
		add_binaryOperators<CVec, real, complex>(builder, "*", fmul);

		add_binaryOperators<RVec, RVec, CVec>(builder, "*", fpwmul);
		add_binaryOperators<CVec, RVec, CVec>(builder, "*", fpwmul);

		add_binaryOperators<real, real, complex, RVec, CVec>(builder, "/", fdiv);
		add_binaryOperators<complex, real, complex, RVec, CVec>(builder, "/", fdiv);
		add_binaryOperators<RVec, real, complex, RVec, CVec>(builder, "/", fdiv);
		add_binaryOperators<CVec, real, complex, RVec, CVec>(builder, "/", fdiv);

		auto fpowScalar = [](const auto& a, const auto& b) { return pow(a, b); };
		auto fpowVector = [](auto v, const auto& a)
		{
			for(auto i = 0u;i<v.size();++i)
			{
				v.at(i) = std::pow(v.at(i), a);
			}

			return v;
		};
		builder.setOperator<real, real>("^", fpowScalar);
		builder.setOperator<complex, real>("^", fpowScalar);
		builder.setOperator<RVec, real>("^", fpowVector);
		builder.setOperator<CVec, real>("^", fpowVector);

		// default functions
		// setFunction takes input parameters as template parameters, then name, and finally  a lambda containing the implementation.

		builder.setFunction<real, real>("pow", fpowScalar);
		builder.setFunction<complex, real>("pow", fpowScalar);
		builder.setFunction<RVec, real>("pow", fpowVector);
		builder.setFunction<CVec, real>("pow", fpowVector);

		auto gauss = [](const RVec& x, real front, real width, real center)
			{
				return front * exp(-1 / (2 * width * width) * pow(x - center, 2));
			};
		builder.setFunction<RVec, real, real, real>("gauss", gauss);

		// single-valued functions for all types easily
		auto addSingleFunc = [&builder](const std::string& name, auto F)
			{
				builder.setFunction<real>(name, F);
				builder.setFunction<complex>(name, F);
				builder.setFunction<RVec>(name, F);
				builder.setFunction<CVec>(name, F);
			};

		// misc
		addSingleFunc("exp", [](const auto& a) { return exp(a); });
		addSingleFunc("sqrt", [](const auto& a) { return sqrt(a); });

		builder.setFunction<real>("abs", [](const auto& a) { return std::abs(a); });
		builder.setFunction<complex>("abs", [](const auto& a) { return std::abs(a); });
		builder.setFunction<RVec>("abs", [](const auto& a) { return abs(a); });
		builder.setFunction<CVec>("abs", [](const auto& a) { return abs(a); });

		builder.setFunction<real>("exp2", [](const auto& a) { return exp2(a); });
		builder.setFunction<RVec>("exp2", [](const auto& a) { return exp2(a); });

		builder.setFunction<real>("log", [](const auto& a) { return log(a); });
		builder.setFunction<RVec>("log", [](const auto& a) { return log(a); });

		builder.setFunction<real>("log2", [](const auto& a) { return log2(a); });
		builder.setFunction<RVec>("log2", [](const auto& a) { return log2(a); });

		builder.setFunction<real>("log10", [](const auto& a) { return log10(a); });
		builder.setFunction<RVec>("log10", [](const auto& a) { return log10(a); });

		builder.setFunction<complex>("re", [](const auto& a) { return std::real(a); });
		builder.setFunction<CVec>("re", [](const auto& a) { return re(a); });
		builder.setFunction<complex>("im", [](const auto& a) { return std::imag(a); });
		builder.setFunction<CVec>("im", [](const auto& a) { return im(a); });

		// trig
		addSingleFunc("sin", [](const auto& a) { return sin(a); });
		addSingleFunc("cos", [](const auto& a) { return cos(a); });
		addSingleFunc("tan", [](const auto& a) { return tan(a); });
		addSingleFunc("asin", [](const auto& a) { return asin(a); });
		addSingleFunc("acos", [](const auto& a) { return acos(a); });
		addSingleFunc("atan", [](const auto& a) { return atan(a); });
		addSingleFunc("sinh", [](const auto& a) { return sinh(a); });
		addSingleFunc("cosh", [](const auto& a) { return cosh(a); });
		addSingleFunc("tanh", [](const auto& a) { return tanh(a); });
		addSingleFunc("asinh", [](const auto& a) { return asinh(a); });
		addSingleFunc("acosh", [](const auto& a) { return acosh(a); });
		addSingleFunc("atanh", [](const auto& a) { return atanh(a); });
	}
}
