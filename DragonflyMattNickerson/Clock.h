#pragma once
#include <cstdint>


namespace df {


	class Clock {
	private:
		long long m_previous_time; // Previous time delta() called (in microsec).
	public:
		// Sets previous time to current time.
		Clock();


		// Resets previous time. Units are microseconds.
		long long delta();


		// Does not reset previous time. Units are microseconds.
		long long split() const;
	};


}