
#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "EventStep.h"
#include "Clock.h"
#include <Windows.h>
#include <mmsystem.h>

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

  // Improve Sleep() resolution for this process (~1ms).
  timeBeginPeriod(1);

  game_over = false;

  Manager::startUp();
  LogManager::getInstance().writeLog("GameManager started\n");
  return 0;
}

void GameManager::shutDown() {
  LogManager::getInstance().writeLog("GameManager shutting down\n");
  game_over = true;

  timeEndPeriod(1);

  Manager::shutDown();
}

void GameManager::setGameOver(bool new_game_over) { game_over = new_game_over; }
bool GameManager::getGameOver() const { return game_over; }
int  GameManager::getFrameTime() const { return frame_time; }

void GameManager::run() {
  if (!isStarted()) return;

  LogManager::getInstance().writeLog("Game loop starting\n");

  Clock clock;                       // microsecond timer
  long long adjust_us = 0;           // oversleep adjustment (fine tuning)
  const long long target_us = static_cast<long long>(frame_time) * 1000LL;

  int step_count = 0;
  while (!game_over) {
    clock.delta(); // start timing for this iteration

    EventStep evt(step_count);
    auto objs = WM().getAllObjects();                 // copy: safe for deferred deletes
    for (int i = 0; i < objs.getCount(); ++i) {
      Object* o = const_cast<Object*>(objs[i]);       // const overload returns const*
      if (o) o->onEvent(evt);
    }
    WM().update(); // process deletions


    // Measure loop body time and sleep remaining budget
    long long loop_time = clock.split();              // µs since delta()
    long long intended_sleep = target_us - loop_time - adjust_us;

    if (intended_sleep > 0) {
      clock.delta();
      sleepMicros(intended_sleep);                    // Sleep()
      long long actual_sleep = clock.split();
      adjust_us = actual_sleep - intended_sleep;      // fine-tune next frame
      if (adjust_us < 0) adjust_us = 0;
    } else {
      adjust_us = 0;
    }

    ++step_count;
  }

  LogManager::getInstance().writeLog("Game loop ended\n");
}

} 
