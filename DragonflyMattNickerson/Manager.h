#pragma once
#include <string>

class Event;

namespace df {

	class Manager {
	private:
		std::string m_type; // Manager type identifier
		bool m_is_started; // True when started successfully


	protected:
		// Set type identifier of Manager 
		void setType(std::string type);


	public:
		Manager();
		virtual ~Manager();


		// Get type identifier of Manager.
		std::string getType() const;


		// Startup Manager. Return 0 if ok, else negative number.
		virtual int startUp();


		// Shutdown Manager.
		virtual void shutDown();


		// Return true when startUp() was executed ok, else false.
		bool isStarted() const;

		// Handle event (default does nothing). Return 0 if ignored, else 1.
		virtual int onEvent(const Event& e) { (void)e; return 0; }

	};


}