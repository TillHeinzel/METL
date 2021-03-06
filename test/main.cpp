
#include <gtest/gtest.h>
#include <chrono>

#include "WriteTime.hpp"

int main(int argc, char **argv) {

	::testing::InitGoogleTest(&argc, argv);

	const auto start = std::chrono::steady_clock::now();
	const int result = RUN_ALL_TESTS();
	const auto end = std::chrono::steady_clock::now(); 
	
	const unsigned int elapsed_seconds = static_cast<unsigned int>((end - start).count()/ std::chrono::steady_clock::period::den);

	std::cout << "Tests took: ";
	writeTime(elapsed_seconds);

#if WIN32
	system("pause");
#endif

	return result;
}
