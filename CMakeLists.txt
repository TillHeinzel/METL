cmake_minimum_required (VERSION 3.3)
project (METL)

#############################################
########## SETTINGS AND DEFAULTS ############
#############################################

include(settings.txt OPTIONAL)

if(NOT DEFINED PEGTL_INCLUDE_DIR)
    set(PEGTL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/PEGTL/include")
endif()

if(NOT DEFINED BUILD_TESTS)
	set(BUILD_TESTS false)
endif()

if(NOT DEFINED BUILD_EXAMPLE_PROJECTS)
	set(BUILD_TESTS false)
endif()

if(${BUILD_TESTS} AND NOT DEFINED GTEST_INCLUDE_DIR OR NOT DEFINED GTEST_LIB_DIR)
	message(ERROR "trying to build test without setting include and lib dirs for googletest")
endif()

####################################
########### OUTPUT PATHS ###########
####################################

if(${PLACE_BINARIES_IN_BUILD_FOLDER})
	message(STATUS "binaries are placed in build-folder")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
else()
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()

####################################
########## GLOBAL DEFINES ##########
####################################

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	set(GCC true)
else()
	set(GCC false)
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CLANG true)
else()
	set(CLANG false)
endif()


if(MSVC)
	add_definitions("/MP")
	# add_definitions("/Za")
	add_definitions("/bigobj")
	
	add_definitions("/W4")
	add_definitions("/WX")

elseif(GCC)
	add_definitions("-Wall -Wno-long-long")
elseif(CLANG)
	add_definitions("-Weverything")
endif()

set(METL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

###################################################
##################### LIBRARY #####################
###################################################

add_subdirectory(include/src) #its a header-only template library, but this generates projects for IDEs

###########################################################
##################### TESTS& EXAMPLES #####################
###########################################################
if(BUILD_EXAMPLE_PROJECTS)
	add_subdirectory(examples)
endif()

if(BUILD_TESTS)
	include_directories("${GTEST_INCLUDE_DIR}")
	link_directories("${GTEST_LIB_DIR}")
	add_subdirectory(test)
endif()
