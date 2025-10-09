#include "Clock.h"
#include <chrono>


namespace df {
	// Return current time in microseconds.
	namespace {
		inline long long nowMicros() {
			using clock = std::chrono::steady_clock;
			auto now = clock::now().time_since_epoch();
			return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
		}
	}


	Clock::Clock() : m_previous_time(nowMicros()) {}

	// Return time in microseconds since last call to delta() and update previous time.
	long long Clock::delta() {
		const long long now = nowMicros();
		const long long dt = now - m_previous_time;
		m_previous_time = now;
		return (dt >= 0) ? dt : -1;
	}

	// Return time in microseconds since last call to delta() but do NOT update previous time.
	long long Clock::split() const {
		const long long now = nowMicros();
		const long long dt = now - m_previous_time;
		return (dt >= 0) ? dt : -1;
	}


}