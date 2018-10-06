cmake_minimum_required (VERSION 3.3)
project (metl CXX)

file(GLOB_RECURSE SOURCES *.h)
file(GLOB ROOT_FILES *.h *.cpp)

include_directories(${METL_ROOT_DIR}/include)

# target for "linking" to
add_library(metl INTERFACE)
target_include_directories(metl INTERFACE "${PEGTL_INCLUDE_DIR}")
target_include_directories(metl INTERFACE "${METL_ROOT_DIR}/dependencies/Hana")
target_include_directories(metl INTERFACE "${METL_ROOT_DIR}/include")

# target for showing headers in IDE
add_custom_target(metl_impl SOURCES ${SOURCES} ../metl.h)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
	source_group("" FILES ${ROOT_FILES} ../metl.h)
endif()
