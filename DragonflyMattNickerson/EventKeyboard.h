#pragma once
#include "Event.h"
#include <string>


class EventKeyboard : public Event {
	int m_key{ 0 };
public:
	static const std::string TYPE; // "keyboard"

	explicit EventKeyboard(int key = 0) : Event(TYPE), m_key(key) {}

	void setKey(int key) { m_key = key; }
	int  getKey() const { return m_key; }
};