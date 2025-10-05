#include "Clock.h"


Clock::Clock() : m_prev(clock::now()) {}
void Clock::reset() { m_prev = clock::now(); }
long long Clock::delta() {
	auto now = clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_prev).count();
	m_prev = now;
	return ms;
}
long long Clock::split() const {
	auto now = clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_prev).count();
}