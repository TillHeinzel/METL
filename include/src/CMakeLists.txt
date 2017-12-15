cmake_minimum_required (VERSION 3.3)
project (metl)

set(SOURCES
	../metl.h

	Compiler.fwd.h
	Compiler.impl.h
	Compiler.h
	
	addDefaults.h
	
	VarExpression.h
	
	nameMangling.h
	grammar.h
	actions.h
	
	TypeList.h
	
	CompilerHelpers.h
	
	Stack.fwd.h
	Stack.impl.h
	
	Compiler_Detail.fwd.h
	Compiler_Detail.impl.h
	
	DeepCopy.h
)

# target for "linking" to
add_library(metl INTERFACE)
target_include_directories(metl INTERFACE "${PEGTL_INCLUDE_DIR}")
target_include_directories(metl INTERFACE "${METL_ROOT_DIR}/include")

# target for showing headers in IDE
add_custom_target(metl_impl SOURCES ${SOURCES})