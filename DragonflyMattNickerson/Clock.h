#include <chrono>


class Clock {
	using clock = std::chrono::steady_clock;
	clock::time_point m_prev;
public:
	Clock();
	void reset();
	long long delta(); // ms since last call and resets
	long long split() const; // ms since last reset
};