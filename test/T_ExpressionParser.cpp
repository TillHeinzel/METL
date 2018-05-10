
#include <gtest/gtest.h>
#include <complex>

#include "metl.h"

class MetlFixture : public ::testing::Test
{
public:
	const double tol = 1e-15;
};

TEST_F(MetlFixture, SetReadConstants)
{
	auto c = metl::makeCompiler<int, double>();

	c.setConstant("a", 1);



	ASSERT_EQ(1, c.build<int>("a")());
	ASSERT_EQ(1, c.getValue<int>("a"));

}

TEST_F(MetlFixture, VarExpressionSet)
{
	auto c = metl::makeCompiler<int, double>();

	auto expr = c.build("1");

	ASSERT_ANY_THROW(expr.get<double>());

	expr.set<double>([]() {return 1.0; });

	ASSERT_EQ(1.0, expr.get<double>()());
}

TEST_F(MetlFixture, buildCast)
{
	auto c = metl::makeCompiler<int, double>();
	metl::setDefaults(c);

	auto expr = c.build("1");

	ASSERT_EQ(1, expr.get<int>()());
	ASSERT_EQ(1.0, expr.get<double>()());
}


TEST_F(MetlFixture, constexprEval)
{
	auto c = metl::makeCompiler<int, double>();
	metl::setDefaults(c);


	bool intPlusCalled = false;
	auto intplus = [&](int left, int right) {intPlusCalled = !intPlusCalled; return left + right; };

	c.setOperator<int, int>("+", intplus);

	auto f=c.build<int>("1+1");
	EXPECT_TRUE(intPlusCalled);

	EXPECT_EQ(2, f());
	EXPECT_TRUE(intPlusCalled);
}

class LiteralsFixture: public MetlFixture{};

TEST_F(LiteralsFixture, intOnly)
{
	// test that an int-only compiler can be created.

	auto compiler = metl::makeCompiler<int>();

	ASSERT_EQ(compiler.build<int>("1")(), 1);
	ASSERT_THROW(compiler.build<int>("1.2")(), metl::BadLiteralException);
}

TEST_F(LiteralsFixture, withSpaces)
{
	// test that an int-only compiler can be created.

	auto compiler = metl::makeCompiler<int, double>();
	metl::setDefaults(compiler);

	ASSERT_EQ(compiler.build<int>(" 1 ")(), 1);
}

TEST_F(LiteralsFixture, realOnly)
{
	// test that an int-only compiler can be created.

	auto compiler = metl::makeCompiler<double>();

	ASSERT_EQ(compiler.build<double>("1.0")(), 1.0);
	ASSERT_THROW(compiler.build<double>("1")(), metl::BadLiteralException);
}

TEST_F(LiteralsFixture, user_defined_converter)
{
	// test that an int-only compiler can be created.

	auto ff = [](const std::string& s) ->double {return std::stod(s); };
	//decltype(std::declval<decltype(ff)>() (std::declval<std::string>())) d;

	//auto conv = metl::makeRealConverter(ff);

	//auto compiler = metl::makeCompiler<double>();
	auto compiler = metl::makeCompiler<double>(metl::makeIntConverter(ff));

	ASSERT_EQ(compiler.build<double>("1")(), 1.0);
	ASSERT_EQ(compiler.build<double>("1.0")(), 1.0);
}



class DefaultsFixture:public MetlFixture{};

TEST_F(DefaultsFixture, defaultIntOperators)
{
	auto compiler = metl::makeCompiler<int>();

	//ASSERT_EQ(compiler.build<int>("1")(), 1); //assert literals work

	metl::addDefaultOperators<int>(compiler);
	ASSERT_EQ(compiler.build<int>("+1")(), 1);
	ASSERT_EQ(compiler.build<int>("-1")(), -1);


	ASSERT_EQ(compiler.build<int>("1+1")(), 2); 
	ASSERT_EQ(compiler.build<int>("3-2")(), 1); 
	ASSERT_EQ(compiler.build<int>("2*3")(), 6);
	ASSERT_EQ(compiler.build<int>("4/2")(), 2); 
	ASSERT_EQ(compiler.build<int>("5/2")(), 2); 
}

TEST_F(DefaultsFixture, defaultDoubleOperators)
{
	auto compiler = metl::makeCompiler<double>();

	//ASSERT_EQ(compiler.build<int>("1")(), 1); //assert literals work

	metl::addDefaultOperators<double>(compiler);
	ASSERT_EQ(compiler.build<double>("+1.0")(), 1);
	ASSERT_EQ(compiler.build<double>("-1.0")(), -1);

	ASSERT_EQ(compiler.build<double>("1.0+1.0")(), 2.0);
	ASSERT_EQ(compiler.build<double>("3.0-2.0")(), 1.0);
	ASSERT_EQ(compiler.build<double>("2.0*3.0")(), 6.0);
	ASSERT_EQ(compiler.build<double>("4.0/2.0")(), 2.0);
	ASSERT_EQ(compiler.build<double>("5.0/2.0")(), 2.5);
}

TEST_F(DefaultsFixture, basicDoubleFunctions)
{
	auto compiler = metl::makeCompiler<double>();

	metl::addDefaultOperators<double>(compiler);
	metl::addBasicFunctions<double>(compiler);

	ASSERT_EQ(compiler.build<double>("exp(2.0)")(), std::exp(2));
	ASSERT_EQ(compiler.build<double>("sqrt(2.0)")(), std::sqrt(2));
	ASSERT_EQ(compiler.build<double>("exp2(2.0)")(), std::exp2(2));
	ASSERT_EQ(compiler.build<double>("log(2.0)")(), std::log(2));
	ASSERT_EQ(compiler.build<double>("log2(2.0)")(), std::log2(2));
	ASSERT_EQ(compiler.build<double>("log10(2.0)")(), std::log10(2));
}

TEST_F(DefaultsFixture, setDefaultsInt)
{
	auto compiler = metl::makeCompiler<int>();

	metl::setDefaults(compiler);

	ASSERT_EQ(compiler.build<int>("1")(), 1);
	ASSERT_THROW(compiler.build<int>("1.0")(), metl::BadLiteralException);


}

class operationsFixture: public MetlFixture{};

TEST_F(operationsFixture, unaryOperators)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>();

	compiler.setUnaryOperatorPrecedence("-", 1);
	auto minus = [](auto a) {return -a; };

	compiler.setUnaryOperator<int>("-", minus);
	compiler.setUnaryOperator<double>("-", minus);
	compiler.setUnaryOperator<std::complex<double>>("-", minus);

	compiler.setSuffix<double, std::complex<double>>("i", [](double d) {return std::complex<double>(0, d); });

	ASSERT_EQ(compiler.build<int>("-1")(), -1);
	ASSERT_EQ(compiler.build<double>("-1.0")(), -1);
	ASSERT_EQ(compiler.build<std::complex<double>>("-1.0i")(), std::complex<double>(0, -1));

}

TEST_F(operationsFixture, binaryOperatorsAndSuffix)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>();

	compiler.setOperatorPrecedence("+", 1);
	auto plus = [](auto left, auto right) {return left + right; };

	compiler.setOperator<int, int>("+", plus);
	compiler.setOperator<int, double>("+", plus);
	compiler.setOperator<double, std::complex<double>>("+", plus);

	compiler.setSuffix<double, std::complex<double>>("i", [](double d) {return std::complex<double>(0, d); });

	ASSERT_EQ(compiler.build<int>("1+1")(), 2);
	ASSERT_EQ(compiler.build<double>("1+1.0")(), 2);
	ASSERT_EQ(compiler.build<std::complex<double>>("1.0+(1.0+1.0i)")(), std::complex<double>(2,1));
}

class UserDefinedLiteralsFixture : public MetlFixture {};

TEST_F(UserDefinedLiteralsFixture, binaryOperatorsAndSuffix)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>(metl::makeIntConverter([](std::string str) {return std::complex<double>{0, std::stod(str)}; }));

	auto ff = compiler.build("2");
	
	auto checkAgainst = std::complex<double>{ 0, 2 };
	auto result = ff.get<std::complex<double>>()();

	ASSERT_EQ(checkAgainst, result);
}

class AssignmentFixture : public MetlFixture {};


TEST_F(AssignmentFixture, assignLiteralToNewConstant)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>();

	metl::setDefaults(compiler);

	auto ff = compiler.build<int>("a = 2");

	ASSERT_EQ(2, compiler.getValue<int>("a"));
}

TEST_F(AssignmentFixture, assignLiteralToExistingConstant)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>();

	metl::setDefaults(compiler);
	compiler.setConstant("a", 1);

	auto ff = compiler.build<int>("a = 2");

	ASSERT_EQ(2, compiler.getValue<int>("a"));
}

TEST_F(AssignmentFixture, assignLiteralToExistingVariable)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>();

	metl::setDefaults(compiler);

	auto a = 1;
	compiler.setVariable("a", &a);

	auto ff = compiler.build<int>("a = 2");

	ASSERT_EQ(2, a);
}

TEST_F(AssignmentFixture, assignExpressionToNewConstant)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>();

	metl::setDefaults(compiler);
	compiler.setConstant("b", 45.0);

	auto x = 3;
	compiler.setVariable("x", &x);

	auto ff = compiler.build<double>("a = 2*x+b");

	x = 4;
	ASSERT_EQ(2*3+45.0, compiler.getValue<double>("a"));
}

TEST_F(AssignmentFixture, assignExpressionToExistingConstant)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>();

	metl::setDefaults(compiler);
	compiler.setConstant("a", 1);
	EXPECT_EQ(1, compiler.getValue<int>("a"));

	compiler.setConstant("b", 45.0);

	auto x = 3;
	compiler.setVariable("x", &x);

	// constants can change type
	auto ff = compiler.build<double>("a = 2*x+b");

	EXPECT_EQ(2 * 3 + 45.0, compiler.getValue<double>("a"));
}

TEST_F(AssignmentFixture, assignExpressionToExistingVariable)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>();

	metl::setDefaults(compiler);

	auto a = 1.0;
	compiler.setVariable("a", &a);

	compiler.setConstant("b", 45.0);

	auto x = 3;
	compiler.setVariable("x", &x);

	// variables can not change type
	EXPECT_ANY_THROW(compiler.build<std::complex<double>>("a = 2i*x+b"));

	compiler.build<double>("a = 2*x+b");
	EXPECT_EQ(2 * 3 + 45.0, compiler.getValue<double>("a"));

	compiler.build<double>("a = 2");
	EXPECT_EQ(2.0, compiler.getValue<double>("a"));
}

TEST_F(AssignmentFixture, disableAssignment)
{
	auto compiler = metl::makeCompiler<int, double, std::complex<double>>(metl::disableAssignment());

	metl::setDefaults(compiler);



	auto val = compiler.build<double>("42")();
	EXPECT_EQ(42.0, val);

	//EXPECT_ANY_THROW(compiler.build("a = 42"));
}
