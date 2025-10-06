#include "Clock.h"
#include <chrono>


namespace df {
	namespace {
		inline long long nowMicros() {
			using clock = std::chrono::steady_clock;
			auto now = clock::now().time_since_epoch();
			return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
		}
	}


	Clock::Clock() : m_previous_time(nowMicros()) {}


	long long Clock::delta() {
		const long long now = nowMicros();
		const long long dt = now - m_previous_time;
		m_previous_time = now;
		return (dt >= 0) ? dt : -1;
	}


	long long Clock::split() const {
		const long long now = nowMicros();
		const long long dt = now - m_previous_time;
		return (dt >= 0) ? dt : -1;
	}


}