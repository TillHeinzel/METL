
#include <gtest/gtest.h>
#include <chrono>

#include "WriteTime.h"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);

	auto start = std::chrono::steady_clock::now();
	int result = RUN_ALL_TESTS();
	auto end = std::chrono::steady_clock::now(); 
	
	unsigned int elapsed_seconds = static_cast<unsigned int>((end - start).count()/ std::chrono::steady_clock::period::den);

	std::cout << "Tests took: ";
	writeTime(elapsed_seconds);
	return result;
}
