
#include <gtest/gtest.h>
#include <complex>

#include "metl.h"

class MetlFixture : public ::testing::Test
{
public:
	const double tol = 1e-15;
};

class CreatonFixture: public MetlFixture{};

TEST_F(CreatonFixture, int_only)
{
	// test that an int-only compiler can be created.

	auto compiler = metl::makeCompiler<int>();

	ASSERT_EQ(compiler.build<int>("1")(), 1);
	ASSERT_THROW(compiler.build<int>("1.2")(), metl::BadLiteralException);
}

/*
TEST_F(CreatonFixture, scalarLiterals)
{

	auto compiler = metl::makeCompiler<int, double, std::complex<double>>();

	compiler.setOperatorPrecedence("+", 2);
	compiler.setUnaryOperatorPrecedence("-", 1);

	auto plusFunc = [](const auto left, const auto right) {return left + right; };
	//compiler.setOperator<int, int>("+", plusFunc);
	compiler.setCast<int>([](const int i) {return static_cast<double>(i); });
	compiler.setOperator<double, double>("+", plusFunc);
	compiler.setOperator<int, int>("+", plusFunc);
	const auto minFunc = [](const auto left) {return -left; };
	compiler.setUnaryOperator<double>("-", minFunc);


	compiler.setSuffix<double, std::complex<double>>("i", [](const double d) {return std::complex<double>(0, d); });
	
	auto func = compiler.build<int>("1+2");
	ASSERT_EQ(func(), 1 + 2);

	auto expr = compiler.build("2+3");
	auto func2 = expr.get<int>();

	ASSERT_EQ(func2(), 2 + 3);

	compiler.setFunction<int>("plusOne", [](int i) {return i + 1; });

	auto func3 = compiler.build<std::complex<double>>("2i");
	std::cout << func3() << std::endl;
	/*

	QExpressionBuilder compiler{};
	addDefaults(compiler);

	std::string test = "2";
	auto expr = compiler.build(test);
	ASSERT_TRUE(expr.type() == QExpressionBuilder::type<int>());
	EXPECT_NEAR(expr.get<int>()(), 2, acc);

	test = "2.3";
	expr = compiler.build(test);
	ASSERT_TRUE(expr.type() == QExpressionBuilder::type<real>());
	EXPECT_NEAR(expr.get<real>()(), 2.3, acc);

	test = "2.3i";
	expr = compiler.build(test);
	ASSERT_TRUE(expr.type() == QExpressionBuilder::type<complex>());
	EXPECT_NEAR(std::real(expr.get<complex>()()), 0.0, acc);
	EXPECT_NEAR(std::imag(expr.get<complex>()()), 2.3, acc);

	test = "-2.3+4.2*2.5-88/3.14^3";
	expr = compiler.build(test);
	ASSERT_TRUE(expr.type() == QExpressionBuilder::type<real>());
	EXPECT_NEAR(expr.get<real>()(), -2.3 + 4.2*2.5 - 88 / std::pow(3.14, 3), acc);

	test = "-2.3i+4.2*2.5-88/3.14^3";
	expr = compiler.build(test);
	ASSERT_TRUE(expr.type() == QExpressionBuilder::type<complex>());
	EXPECT_NEAR(std::real(expr.get<complex>()()), std::real(-2.3*IMAGUNIT + 4.2*2.5 - 88 / std::pow(3.14,3)), acc);
	EXPECT_NEAR(std::imag(expr.get<complex>()()), std::imag(-2.3*IMAGUNIT + 4.2*2.5 - 88 / std::pow(3.14,3)), acc);
}
*/

//
//TEST_F(ExpressionParser_Fixture, gauss)
//{
//	using namespace metl;
//	using namespace qengine;
//
//	QExpressionBuilder compiler{};
//	addDefaults(compiler);
//
//	auto gauss = [](const RVec& x, real a, real width, real center)
//	{
//		return a*exp(-1 / (2 * width*width)*pow(x - center, 2));
//	};
//
//	auto x = qengine::linspace(-2, 2, 128);
//	auto front = 1.0;
//	auto width = 16.0;
//	auto center = 0.0;
//
//	compiler.setConstant("x", x);
//	compiler.setConstant("front", front);
//	compiler.setConstant("width", width);
//
//	compiler.setVariable("center", &center); //center is variable!
//
//	std::string test = "gauss(x, -front, width, center)"; // spaces are free!
//	auto expr = compiler.build(test);
//
//	auto g = gauss(x, -front, width, center);
//	ASSERT_TRUE(expr.type() == QExpressionBuilder::type<RVec>());
//	EXPECT_NEAR(dot(expr.get<RVec>()(), g), dot(g,g), acc);
//}
//
//TEST_F(ExpressionParser_Fixture, usage)
//{
//	using namespace metl;
//	using namespace qengine;
//
//	QExpressionBuilder compiler{};
//	addDefaults(compiler);
//
//
//	auto x = qengine::linspace(-2, 2, 4);
//
//	compiler.setConstant("x", x);
//	compiler.setConstant("mu", 0.0);
//	compiler.setConstant("var", 0.1);
//
//	real s = 0.5;
//	compiler.setVariable("s", &s);
//
//	std::string test = "x*x";
//
//	auto expr = compiler.build(test);
//
//	switch (expr.type())
//	{
//	case QExpressionBuilder::type<real>() : std::cout << expr.get<real>()() << std::endl; break;
//	case QExpressionBuilder::type<complex>() : std::cout << expr.get<complex>()() << std::endl; break;
//	case QExpressionBuilder::type<RVec>() : std::cout << expr.get<RVec>()() << std::endl; break;
//	case QExpressionBuilder::type<CVec>() : std::cout << expr.get<CVec>()() << std::endl; break;
//	default: break;
//	}
//
//	s = 1.5;
//	switch (expr.type())
//	{
//	case QExpressionBuilder::type<real>() : std::cout << expr.get<real>()() << std::endl; break;
//	case QExpressionBuilder::type<complex>() : std::cout << expr.get<complex>()() << std::endl; break;
//	case QExpressionBuilder::type<RVec>() : std::cout << expr.get<RVec>()() << std::endl; break;
//	case QExpressionBuilder::type<CVec>() : std::cout << expr.get<CVec>()() << std::endl; break;
//	default: break;
//	}
//}
//
//
//TEST_F(ExpressionParser_Fixture, speed)
//{
//	using namespace metl;
//	using namespace qengine;
//
//	QExpressionBuilder compiler{};
//	addDefaults(compiler);
//
//	auto x = qengine::linspace(-2, 2, 512);
//	auto front = 1.0;
//	auto width = 16.0;
//	auto center = 0.0;
//
//	compiler.setConstant("x", x);
//	compiler.setVariable("front", &front);
//	compiler.setVariable("width", &width);
//
//	compiler.setVariable("center", &center); //center is variable!
//
//	std::string test = "front*exp(-1.0 / (2.0 * width*width)*pow(x - center,2.0))"; // spaces are free!
//	auto expr = compiler.build(test);
//
//	auto expr_func = expr.get<RVec>();
//	auto native_func = [&x, &front, &width, &center]()
//	{
//		return front*exp(-1 / (2 * width*width)*pow(x - center, 2));
//	};
//
//	auto stepsize = 0.001;
//
//	auto checkNative = 0.0;
//	auto t0 = std::chrono::high_resolution_clock::now();
//	for(auto c = -1.0; c<1.0; c+= stepsize)
//	{
//	/*	center = c;
//		front = 1.0 + 0.5*c;
//		width = 16.0 + c;*/
//		auto s = native_func();
//		checkNative += s.at(0);
//	}
//	auto t1 = std::chrono::high_resolution_clock::now();
//	auto dtNative = 1.e-9*std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
//
//	auto checkExpr = 0.0;
//	auto t3 = std::chrono::high_resolution_clock::now();
//	for (auto c = -1.0; c<1.0; c += stepsize)
//	{
//	/*	center = c;
//		front = 1.0 + 0.5*c;
//		width = 16.0 + c;*/
//		auto s = expr_func();
//		checkExpr += s.at(0);
//	}
//	auto t4 = std::chrono::high_resolution_clock::now();
//	auto dtExpr = 1.e-9*std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
//
//	std::cout << "check native: " << checkNative << " check expr: " << checkExpr << "; time native: " << dtNative << " time expr: " << dtExpr << std::endl;
//}
//
//
//TEST_F(ExpressionParser_Fixture, casting)
//{
//	using namespace metl;
//	using namespace qengine;
//
//
//	QExpressionBuilder compiler{};
//	addDefaults(compiler);
//	
//	std::string test = "2+2";
//	auto expr = compiler.build(test);
//	ASSERT_EQ(expr.get<int>()(), 4);
//}
//TEST_F(ExpressionParser_Fixture, powOperator)
//{
//	using namespace metl;
//	using namespace qengine;
//
//	QExpressionBuilder compiler{};
//	compiler.setOperatorPrecedence("^", 1);
//	compiler.setOperator<double, double>("^", [](auto left, auto right) {return pow(left, right); });
//
//	std::string test = "2.0^2.0";
//	auto expr = compiler.build(test);
//	ASSERT_NEAR(expr.get<real>()(), 4.0, 1e-16);
//
//}
//TEST_F(ExpressionParser_Fixture, unaryOperators)
//{
//	using namespace metl;
//	using namespace qengine;
//
//	QExpressionBuilder compiler{};
//	compiler.setOperatorPrecedence("^", 1, ASSOCIATIVITY::LEFT);
//	compiler.setUnaryOperatorPrecedence("!", 2);
//	compiler.setOperator<int, int>("^", [](auto left, auto right) {return pow(left, right); });
//	compiler.setUnaryOperator<double>("!", [](auto a) {return -a; });
//
//	std::string test = "!2^2";
//	auto expr = compiler.build(test);
//	ASSERT_NEAR(expr.get<double>()(), -4, 1e-16);
//}
//TEST_F(ExpressionParser_Fixture, powDefault)
//{
//	using namespace metl;
//	using namespace qengine;
//
//	QExpressionBuilder compiler{};
//	addDefaults(compiler);
//
//	auto v = RVec{ 4,9 };
//	compiler.setConstant("v", v);
//	std::string test = "v^(1/2)";
//	auto expr = compiler.build(test);
//	auto ret = expr.get<RVec>()();
//	EXPECT_NEAR(2.0, ret.at(0), 1e-16);
//	EXPECT_NEAR(3.0, ret.at(1), 1e-16);
//}
//TEST_F(ExpressionParser_Fixture, sqrt)
//{
//	using namespace metl;
//	using namespace qengine;
//
//	QExpressionBuilder compiler{};
//	addDefaults(compiler);
//
//	auto v = RVec{ 4,9 };
//	std::string test = "sqrt(2i)";
//	auto expr = compiler.build(test);
//	auto ret = expr.get<complex>()();
//	std::cout << ret << std::endl;
//}

