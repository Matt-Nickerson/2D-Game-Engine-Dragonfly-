#include "LogManager.h"
#include <cstdarg>
#include <cstdio>
#include <iostream>


LogManager& LogManager::getInstance() {
	static LogManager inst;
	return inst;
}


int LogManager::startUp(const std::string& filename) {
	if (isStarted()) return 0;
	m_file.open(filename.c_str(), std::ios::out | std::ios::trunc);
	if (!m_file.is_open()) {
		std::cerr << "[LogManager] Failed to open log file: " << filename << "";
			return -1;
	}
	started = true;
	log("LogManager started");
	return 0;
}


void LogManager::shutDown() {
	if (!isStarted()) return;
	log("LogManager shutting down");
	m_file.flush();
	m_file.close();
	started = false;
}


void LogManager::log(const std::string& message) {
	if (m_file.is_open()) {
		m_file << message << "";
			m_file.flush();
	}
	else {
		std::cerr << message << "";
	}
}


void LogManager::logf(const char* fmt, ...) {
	constexpr size_t BUF = 1024;
	char buffer[BUF];
	va_list args; va_start(args, fmt);
#if defined(_MSC_VER)
	vsnprintf_s(buffer, BUF, _TRUNCATE, fmt, args);
#else
	vsnprintf(buffer, BUF, fmt, args);
#endif
	va_end(args);
	log(std::string(buffer));
}