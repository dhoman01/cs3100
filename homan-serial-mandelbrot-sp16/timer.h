#include <algorithm>
#include <chrono>

namespace timer {
	template<typename F>
	auto timeFunction(F f) {
		// Start the timer
		auto start = std::chrono::steady_clock::now();
		
		f();        // Call the function being timed
		
		// Stop the timer
		auto end = std::chrono::steady_clock::now();
		return end - start;        // Return the time_span
	}
}

