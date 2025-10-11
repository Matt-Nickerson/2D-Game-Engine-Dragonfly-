#pragma once
#include "Event.h"
#include <string>

class EventOut : public Event {
public:
	static const std::string TYPE;
	EventOut() : Event(TYPE) {}
};