# Math Expression Template Library (METL)

A small, header-only c++14 library for parsing math expressions, built on top of [PEGTL](https://github.com/taocpp/PEGTL).

METL is designed to be very flexible while being reasonably efficient. 
Flexibility means that metl can be used with any reasonably well-behaved types (useful for e.g. vectors or matrices) and that adding or editing operators and functions is very easy. 
It is also header-only, making it easy to integrate with other projects, and has only a single dependency, PEGTL. 
METL is very young, but is now included in a [benchmarking project](https://github.com/ArashPartow/math-parser-benchmark-project) and does reasonably well for scalars. 
However, if you don't need the flexibility of METL I would propose you consider [ExprTK](http://www.partow.net/programming/exprtk/), which is probably the fastest out there for standard operations on scalars.

Current features:
*  build once, call often: METL compiles your expression to a std::function that you can call as often as you want
*  Extremely flexible: You can create any operator or function you want and use it with any type you want. 
*  builds with msvc, gcc, and clang
*  supports literals with or without suffixes and arbitrarily named variables and constants
*  a smart setup for defaults which recognizes standard types (integral and floating point)
*  binary operators, unary operators, functions 
*  built-in assignment operator to new or existing variables and constants

## Introduction

METL is very flexible in that it can work with any types and any functions or operators you can come up with.
It is most useful for "compile once, invoke often" types of situation:


```c++
auto compiler = metl::makeCompiler<int, double, bool, MyVector>()

compiler.setOperatorPrecedence("+", 4);
compiler.setOperator<double, MyVector>("+", [](auto left, auto right){return left+right;});

auto v = MyVector{1,2};
compiler.setVariable("v", &v);

auto f = compiler.build<MyVector>("2.0+v");

auto w = f(); // w == MyVector{3,4}
v = MyVector{2,3};
w = f(); // w == MyVector{4,5}


```

(the MyVector-class is a placeholder and not actually part of the library)



## Installation & dependencies
As METL is header-only, no setup is necessary. 
The only dependency is the PEGTL, which is also header-only and is included as a git submodule for your (and my) convenience.

currently, METL is only tested on MSVC (VS 15.5.6).

## Running the tests & example programs
Sadly, the tests and example programs require compilation. 
First of all, the system is built with cmake. To change the settings for the build, copy the settings_template.txt 
and rename the copy to settings.txt. The cmakeLists will then include this file. In settings.txt you can 
turn DO_TESTS and CREATE_EXAMPLE_PROJECTS on or off. 
You can also set the PEGTL-directory (only if you did not download it as a submodule).

With this you should be able to build and run the example project (which is woefully minimal right now).

To run the tests you need to install [google test](https://github.com/google/googletest).
After having installed google test, you put the include- and lib-directories to googletest into the settings.txt-file and you should be good to go.

## Reference

### metl-compiler
The reference for METL is basically the reference for the metl-compiler.
The metl-compiler is constructed with a factory-function:
```c++
auto compiler = metl::compiler<Types...>(literalConverters...);
```

The Types... is a list of types that the metl-compiler should be able to deal with. 
I have not tested it, but pretty much any well-behaved type should be possible. I think it should be copyable.
You can also pass in any literalConverters. These are functors that are used to interpret literals, so strings like 
"1" and "42.0". If int and double are part of the types of your metl-compiler, the default interpretation is to convert literals to these types.
But if you want to use e.g. long and float instead, you will have to provide your own converters:
```c++

auto longConverter = [](std::string s){return std::stol(s);};
auto floatConverter = [](std::string s){return std::stof(s);};
auto compiler = metl::compiler<long, float>(metl::intConverter(longConverter), metl::realConverter(floatConverter));
```

If you do not use the default types and do not add any custom converters, the metl-compiler will throw a BadLiteralException
when trying to parse a literal. 

note: Defaults are added for each individual type, so if you use int and float, you will get the correct behaviour for int but have to provide you own for the float.

note: Currently, the only literals are actually int and real

### build
The member-function "build" is used to parse a string and build a math-expression. It comes in two varieties:
```c++
compiler.build("a+2");
compiler.build<int>("a+2")
```
The first version returns a VarExpression (basically a variant with different kinds of std::function). This VarExpression can be used in a switch like so:
```c++
auto expression = compiler.build("a+2");

switch(expression.type())
{
    case compiler.type<int>():
        std::cout << expression.get<int>() << std::endl; break;
        
        
    case compiler.type<double>():
        std::cout << expression.get<double>() << std::endl; break;
        
    default: breakl
}

```
This is useful in the case where you do not actually know the type, which should be the common case for when the strings are supplied at runtime.

The second version of the build-function just automatically calls the get<Type>() function to return the std::function directly. 

### binary operators

Operators can be any string-sequence, although I would advise against doing weird stuff.
Each operator has a fixed precedence and associativity. These have to be set for any operator by calling setOperatorPrecedence:
```c++
compiler.setOperatorPrecedence("+", 6, metl::ASSOCIATIVITY::LEFT);
```
Associativity is left per default and can be left out if the operator is left-associative.

After this operator is part of the compiler, specific implementations can be added for specific types:
```c++
compiler.setOperator<int, int>("+", [](auto left, auto right){return left+right;});
```
We have to explicitly set the types for the left and right hand sides and a function that takes two parameters and returns something. 
The return value is inferred from the functor.

Now we can do e.g. 

```c++
compiler.build<int>("1 + 2"); 
```

### unary operators/prefixes

in addition to binary operators we can also add unary operators/prefixes:
```c++
compiler.setUnaryOperatorPrecedence("-", 3);
compiler.setUnaryOperator<int>("-", [](auto i){return -i;});
compiler.build<int>("-1.0");
```
All unary operators are right-associative

### suffixes

suffixes can also be defined, but work only on literals, similar to normal c++:
```c++
compiler.setSuffix<double>("i", [](auto d){return std::complex<double>(d);});
compiler.build<std::complex<double>>("1.0i");
```

### functions
function have the structure "functionName(p1,p2,p3,...)". They are created as

```c++

compiler.setFunction<double>("sin", [](auto d){return std::sin(d);});
compiler.build<double>("sin(3.1415)");

compiler.setConstant("true", true);
compiler.setFunction<double, double, bool>("lifeInPlastic", [](auto barbie, auto ken, auto car){ return car ? barbie+ken : barbie-ken;});
compiler.build<double>("lifeInPlast(4.0,2.0,true)");
```

### casting
sometimes we do not want to define everything twice. So we can define implicit casts:
```c++
compiler.setCast<int>( [](auto i){return double(i);});
compiler.build<double>("sin(3)");
```

### Constants and Variables

We can pass in constants:
```c++
compiler.setConstant("PI", 3.1415);
compiler.build<double>("sin(PI)");
```
and variables:
```c++
auto var = 2.0;
compiler.setVariable("a", &var);
auto f = compiler.build<double>("sin(a)");
f(); // result is sin(2.0)
var = 3.0;
f(); // result is sin(3.0)
```
For variable we pass in address of the variable, so the resulting function (here f) holds that address. 
When building a function that refers to a local c++-variable, remember that function will start behaving weirdly if the pointed-to variable is freed up.

You can access the value of a variable or constant in the compiler by calling
```c++
compiler.getValue<double>("a");
```

### parentheses
parenthesis work as in normal math and are built-in. So e.g. "(1+2)*3" gives 9.0.

### Assignment
METL can handle assignment-expressions ("identifier = {expression}"):
```c++
auto ff = compiler.build<double>("a = 2.0*b+x");
```

This will have one of three results:
1) if identifier does not exist yet, a new constant is created
2) if identifier is a constant, that constant is replaced
3) if identifier is a variable, the value of the pointed-to variable is changed

Important: The assignment only happens during the build! The expression is evaluated and assigned to the constant/variable, but the created function will be equivalent to the one created by the expression "2.0*b+x".
If you want to assign a result again, you will have to do so more directly through setConstant, or by changing the c++variable a METL-variable is pointing to.

## reserved keywords
In its current state, the metl-compiler can take any labels for operators and functions. At some point, there may be more stringent rules for this, but for now I advice against using:
* parenthesis (any kinds really: () [] {} )
* @ (similar to MSVC, I use this as separator for name-mangling)

## Expression-tree building and constexpr-optimization

The metl-compiler constructs the expression-tree when the build-function is called. The results in a std::function that returns the result of the expression.
There is no type-flexibility when the function is actually called. Any flexibility is removed during the build-process. I think this makes it strongly typed.
So when actually invoking the function, we only get the overhead of the type erasure of nested std::functions.

In addition, the metl-compiler knows that certain expressions are actually fixed at build-time, and evaluates them at that point.
This means that the constant parts of the tree get evaluated at build-time, not at call-time. (this is really hard to write without getting confused with the actuall C++-compiler)

so let's say we have an expression involving both constants and variables (literals count as constants):
```c++
auto var = 2.0;
compiler.setVariable("a", &var);
compiler.setConstant("PI", 3.1415);
auto f = compiler.build<double>("sin(a) + cos(2*PI)");
```

The part "cos(2*PI)" will be calculated at build-time, so calling f will be equivalent to calling
"sin(a) + b", where b has been precalculated as cos(2*3.1415).

## Missing stuff
A few things I would like to add at some point, in no particular order:

* While vectors and matrices and such can easily be used as variables and constants, there is no way to create them inline. 
* Errors are currently just passed through from PEGTL (mostly).
* Some limitations on what names can be used, to avoid possible inconsistencies
* some debugging help maybe?
* Whatever issues spring up from people actually using the library