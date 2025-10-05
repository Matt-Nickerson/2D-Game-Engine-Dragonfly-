#include "Manager.h"


namespace df {

	// Constructor
	Manager::Manager() : m_type(""), m_is_started(false) {}
	Manager::~Manager() = default;

	// Setting type to m_type
	void Manager::setType(std::string type) { m_type = std::move(type); }

	// Getter for m_type
	std::string Manager::getType() const { return m_type; }

	// start up engine
	int Manager::startUp() {
		m_is_started = true;
		return 0;
	}

	//shut down engine
	void Manager::shutDown() {
		m_is_started = false;
	}

	// returns true if started, else false
	bool Manager::isStarted() const { return m_is_started; }


}

