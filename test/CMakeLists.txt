cmake_minimum_required (VERSION 3.3)
project (metl-testing)

if(MSVC)
	add_definitions("/bigobj")
endif()

file(GLOB SOURCES *.h *.cpp)

add_executable(metl-testing ${SOURCES} )

target_link_libraries(metl-testing metl)
target_link_libraries(metl-testing debug gtestd debug gtest_maind)
target_link_libraries(metl-testing optimized gtest optimized gtest_main)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(metl-testing -lpthread)
endif()
