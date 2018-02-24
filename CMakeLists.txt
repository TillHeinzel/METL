cmake_minimum_required (VERSION 3.3)
project (METL)

####################################
########### OUTPUT PATHS ###########
####################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)



####################################
########## GLOBAL DEFINES ##########
####################################

add_definitions(-std=c++14)

if(MSVC)
	add_definitions("/MP")
	# add_definitions("/Za")
	add_definitions("/W4")
	add_definitions("/bigobj")
elseif(CMAKE_COMPILER_IS_GNUCXX )
	add_definitions("-Wall -Wno-long-long -fpermissive")
elseif(CMAKE_CXX_COMPILER_ID EQUAL Clang)
	add_definitions("-Weverything")
endif()

####################################
########## DEPENDENCIES ############
####################################

include(settings.txt)

if(NOT PEGTL_INCLUDE_DIR)
    set(PEGTL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/PEGTL/include")
endif()

###################################################
##################### LIBRARY #####################
###################################################

set(METL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(include/src) #its a header-only template library, so I am putting this in to have a project in Visual Studio

###########################################################
##################### TESTS& EXAMPLES #####################
###########################################################
if(DO_TESTS)
	include_directories("${GTEST_INCLUDE_DIR}")
	link_directories("${GTEST_LIB_DIR}")
	add_subdirectory(test)
endif()

if(CREATE_EXAMPLE_PROJECTS)
	add_subdirectory(examples)
endif()