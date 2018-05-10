
#include <iostream>
#include <metl.h>
/*
 * This is a simple tutorial-example to showcase basic functionality. It is not complete
 */

int main()
{
	// create a simple metl-compiler: (I will try to consistently call it that, so it doesn't get confused with the C++-compiler)
	auto compiler = metl::makeCompiler<int>();

	// Thats it! this metl-compiler doesn't do much, but it can interpret simple string-literals:
	auto f = compiler.build<int>("1");

	//// the build<type>-function creates a std::function<type(void)>. This is one of the design-choices for this library:
	//// It is made for "compile once, call often"-kinds of situations. 

	//// To actually get the value, we just call the constructed function:

	//std::cout << "result is: " << f() << std::endl; // prints 1 (hopefully)

	//// This is of course pretty useless so far. We can change that by adding some operators:
	//compiler.setOperatorPrecedence("+", 1, metl::ASSOCIATIVITY::LEFT);
	//compiler.setOperator<int, int>("+", [](auto left, auto right) {return left + right; });

	//f = compiler.build<int>("1+3");
	//std::cout << "second result is: " << f() << std::endl; // 4

	//// The metl-compiler allows you to set precedence and associativity and define binary operators between any two types, simply by
	//// using lambdas. 
	//// However, in many situations we just want default behaviour. For those cases I have made a few optional defautls that can be added
	//// to a metl-compiler, e.g. 
	//metl::addDefaultOperators<int>(compiler);
	//// which adds default binary and unary operators for integers to the compiler

	//f = compiler.build<int>("");

}
