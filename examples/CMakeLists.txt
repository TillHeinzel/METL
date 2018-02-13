cmake_minimum_required (VERSION 3.3)
project (metl-examples)

if(MSVC)
	add_definitions("/bigobj")
endif()

add_executable(example example.cpp )
target_link_libraries(example metl)
