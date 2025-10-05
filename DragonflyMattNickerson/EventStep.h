#include "Event.h"


class EventStep : public Event {
	int m_step = 0;
public:
	explicit EventStep(int step = 0) : Event("STEP"), m_step(step) {}
	int getStepCount() const { return m_step; }
	void setStepCount(int s) { m_step = s; }
};