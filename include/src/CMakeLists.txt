cmake_minimum_required (VERSION 3.3)
project (metl)

set(SOURCES
	../metl.h

	nameMangling.h
	grammar.h
	actions.h
	
	VarExpression.h
	TypeList.h
	
	CompilerHelpers.h
	
	Stack.fwd.h
	Stack.impl.h
	
	Compiler.fwd.h
	Compiler.impl.h
	Compiler.h
	#Compiler.cpp
	
	#QCompiler.h
	#QCompiler.cpp

	DeepCopy.h
)

# target for "linking" to
add_library(metl INTERFACE)
target_include_directories(metl INTERFACE "${PEGTL_INCLUDE_DIR}")
target_include_directories(metl INTERFACE "${METL_ROOT_DIR}/include")

# target for showing headers in IDE
add_custom_target(metl_impl SOURCES ${SOURCES})