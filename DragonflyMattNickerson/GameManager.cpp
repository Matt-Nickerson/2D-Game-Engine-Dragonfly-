
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "EventStep.h"
#include "Clock.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include <Windows.h>

namespace df {

namespace {
// Sleep helper in microseconds (round up to ms for Sleep()).
inline void sleepMicros(long long usec) {
  if (usec <= 0) return;
  DWORD ms = static_cast<DWORD>((usec + 999) / 1000);
  ::Sleep(ms);
}
}

GameManager::GameManager()
  : game_over(true),
    frame_time(FRAME_TIME_DEFAULT) {
  setType("GameManager");
}

GameManager& GameManager::getInstance() {
  static GameManager inst;
  return inst;
}

int GameManager::startUp() {
  if (isStarted()) return 0;

  // Improve Sleep() resolution for this process

  game_over = false;

  Manager::startUp();
  LogManager::getInstance().writeLog("GameManager started\n");
  return 0;
}

void GameManager::shutDown() {
  LogManager::getInstance().writeLog("GameManager shutting down\n");
  game_over = true;


  Manager::shutDown();
}

void GameManager::setGameOver(bool new_game_over) { game_over = new_game_over; }
bool GameManager::getGameOver() const { return game_over; }
int  GameManager::getFrameTime() const { return frame_time; }

void GameManager::run() {
    if (!isStarted()) return;

    LogManager::getInstance().writeLog("Game loop starting\n");

    Clock clock;  // microsecond timer
    long long adjust_us = 0; // oversleep adjustment
    const long long target_us = static_cast<long long>(frame_time) * 1000LL;

    int step_count = 0;
    while (!game_over) {
        clock.delta();   // begin timing this iteration

        InputManager::getInstance().getInput();
        
         EventStep evt(step_count);
         auto objs = WorldManager::getInstance().getAllObjects();  
         for (int i = 0; i < objs.getCount(); ++i) {
             Object* o = const_cast<Object*>(objs[i]);              
             if (o) o->onEvent(evt);
         }
         WorldManager::getInstance().update(); // deferred deletes, moves, etc.
         WorldManager::getInstance().draw();
         DisplayManager::getInstance().swapBuffers();

        long long loop_time = clock.split();
        long long intended_sleep = target_us - loop_time - adjust_us;

        if (intended_sleep > 0) {
            clock.delta();
            sleepMicros(intended_sleep);
            long long actual_sleep = clock.split();
            adjust_us = actual_sleep - intended_sleep;   // carry oversleep to next frame
            if (adjust_us < 0) adjust_us = 0;
        }
        else {
            adjust_us = 0;                                
        }

        ++step_count;
    }

    LogManager::getInstance().writeLog("Game loop ended\n");
}


} 
