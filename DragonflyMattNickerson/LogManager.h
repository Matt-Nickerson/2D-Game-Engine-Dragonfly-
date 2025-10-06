#include <stdio.h>
#include <string>


// Engine includes
#include "Manager.h"


namespace df {


	// Default logfile name
	const std::string LOGFILE_NAME = "dragonfly.log";


	class LogManager : public Manager {
	private:
		LogManager(); // Private since a singleton.
		LogManager(LogManager const&); // Don't allow copy.
		void operator=(LogManager const&); // Don't allow assignment.


		bool m_do_flush; // True if flush to disk after each write.
		FILE* m_p_f; // Pointer to logfile struct.


	public:
		// If logfile is open, close it.
		~LogManager();


		// Get the one and only instance of the LogManager.
		static LogManager& getInstance();


		// Start up the LogManager (open logfile "dragonfly.log").
		int startUp();


		// Shut down the LogManager (close logfile).
		void shutDown();


		// Set flush of logfile after each write.
		void setFlush(bool do_flush = true);


		// Write to logfile. Supports printf() formatting of strings.
		int writeLog(const char* fmt, ...) const;
	};


}