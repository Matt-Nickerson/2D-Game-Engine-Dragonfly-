#include "LogManager.h"
#include <cstdarg>
#include <cstdio>

namespace df {
	// Default log file name.
	LogManager::LogManager()
		: m_do_flush(false), m_p_f(nullptr) {
		setType("LogManager");
	}

	LogManager::~LogManager() {
		// Ensure file is closed if user forgot to call shutDown().
		if (m_p_f) {
			fclose(m_p_f);
			m_p_f = nullptr;
		}
	}

	// Get the one and only instance of the LogManager.
	LogManager& LogManager::getInstance() {
		static LogManager single; // persists across calls
		return single;
	}

	// Start up the LogManager (open logfile "dragonfly.log").
	int LogManager::startUp() {
		if (isStarted()) {
			return 0;
		}
		m_p_f = fopen(LOGFILE_NAME.c_str(), "wt");
		if (!m_p_f) {
			// Can't open log file
			return -1;
		}
		Manager::startUp();
		writeLog("LogManager started");
			return 0;
	}

	// Shut down the LogManager (close logfile).
	void LogManager::shutDown() {
		if (!isStarted()) {
			return;
		}
		writeLog("LogManager shutting down");
			if (m_p_f) {
				fflush(m_p_f);
				fclose(m_p_f);
				m_p_f = nullptr;
			}
		Manager::shutDown();
	}

	// Set flush of logfile after each write.
	void LogManager::setFlush(bool do_flush) { 
		m_do_flush = do_flush; 
	}

	// Write to logfile. Supports printf() formatting of strings.
	int LogManager::writeLog(const char* fmt, ...) const {
		if (!m_p_f) {
			return -1;
		}
		va_list args;
		va_start(args, fmt);
		int written = vfprintf(m_p_f, fmt, args); // printf-style into FILE*
		va_end(args);
		if (written < 0) {
			return -1; // vfprintf error
		}
		if (m_do_flush) {
			fflush(m_p_f);
		}
		return written;
	}


}