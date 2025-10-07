#pragma once
#include "Manager.h"

namespace df {

	const int FRAME_TIME_DEFAULT = 33;

	class GameManager : public Manager {
	private:
		GameManager();
		GameManager(const GameManager&) = delete;              // no copy
		GameManager& operator=(const GameManager&) = delete;   // no copy-assign


		bool game_over; 
		int  frame_time;

	public:
		static GameManager& getInstance();

		int  startUp();
		void shutDown();
		void run();

		void setGameOver(bool new_game_over = true);
		bool getGameOver() const;
		int  getFrameTime() const;
	};

}
