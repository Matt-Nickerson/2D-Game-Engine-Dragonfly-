#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "EventStep.h"
#include "Clock.h"


namespace df {


	namespace {
		// Windows-only sleep helper (microseconds)
		inline void sleepMicros(long long usec) {
			if (usec <= 0) return;
			DWORD ms = static_cast<DWORD>((usec + 999) / 1000); // round up to ensure at least requested
			::Sleep(ms);
		}
	}


	GameManager::GameManager() : game_over(true), frame_time(FRAME_TIME_DEFAULT) {
		setType("GameManager");
	}


	GameManager& GameManager::getInstance() {
		static GameManager inst;
		return inst;
	}


	int GameManager::startUp() {
		if (isStarted()) return 0;


		// Improve Sleep() resolution to ~1ms for the process.
		timeBeginPeriod(1);


		game_over = false;


		Manager::startUp();
		LogManager::getInstance().writeLog("GameManager started");
			return 0;
	}


	void GameManager::shutDown() {
		LogManager::getInstance().writeLog("GameManager shutting down");
			game_over = true;


		timeEndPeriod(1);


		Manager::shutDown();
	}


	void GameManager::setGameOver(bool new_game_over) { game_over = new_game_over; }