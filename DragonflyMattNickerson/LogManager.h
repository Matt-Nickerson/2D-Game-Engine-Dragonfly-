#include "Manager.h"
#include <string>
#include <fstream>
#include <mutex>

class LogManager : public Manager {
private:
	std::ofstream m_file;
	std::mutex m_mtx;
	LogManager() = default;
public:
	LogManager(const LogManager&) = delete;
	LogManager& operator=(const LogManager&) = delete;


	static LogManager& getInstance();


	int startUp(const std::string& filename = "dragonflyA.log");
	void shutDown() override;


	void log(const std::string& message);
	void logf(const char* fmt, ...);
};
