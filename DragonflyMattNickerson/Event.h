#pragma once
#include <string>


class Event {
	std::string m_type;
public:
	explicit Event(std::string type = "") : m_type(std::move(type)) {}
	virtual ~Event() = default;
	const std::string& getType() const { return m_type; }
	void setType(const std::string& t) { m_type = t; }
};