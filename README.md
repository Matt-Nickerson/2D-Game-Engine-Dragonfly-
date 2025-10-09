

Student:  Matthew Nickerson
Login:    mwnickerson@wpi.edu
Course:   IMGD 3000
Date:     9/20/25


1) Platform
-----------
Primary:  Windows 10/11, x64, Visual Studio 2022 (C++17)

No graphics or external libraries are required for Part A. All output is printed to a logfile.


2) What’s Implemented (Part A Requirements)
-------------------------------------------
Managers / lifecycle:
- Manager (base): startup/shutdown, type id, started flag.
- LogManager (singleton): open/close logfile, printf-style logging via writeLog(), optional flush.
- GameManager (singleton): startup/shutdown; **game loop** that sends an EventStep to all Objects each iteration.
- WorldManager (singleton): stores all game Objects, add/remove, list accessors, **deferred batch deletion** via markForDelete()+update().

Core data & types:
- Vector: 2D float vector; get/set, setXY, magnitude, normalize, scale, operator+.
- Object (base): unique ID, type string, world position (Vector). Adds itself to the world in the base constructor, removes itself in the destructor. Virtual onEvent() hook.
- ObjectList: fixed-capacity array of Object*; insert/remove/clear/getCount; operator[] with bounds checks.
- Event / EventStep: base event with type string; derived step event conveys step count.

Testing:
- A single test driver (main) exercises all Part A capabilities and logs PASS/FAIL lines to dragonfly.log.


3) Repository Layout
--------------------
include/
  Manager.h
  LogManager.h
  WorldManager.h
  GameManager.h
  Vector.h
  Object.h
  ObjectList.h
  Event.h
  EventStep.h
  Clock.h

src/
  Manager.cpp
  LogManager.cpp
  WorldManager.cpp
  GameManager.cpp
  Vector.cpp
  Object.cpp
  ObjectList.cpp
  EventStep.cpp
  Clock.cpp
  DragonflyMattNickerson.cpp   (test driver with main())

Makefile           (optional build with g++/clang)
README.txt         (this file)
dragonfly.log      (created at runtime; logfile output)


4) Code Structure & Notes
-------------------------
Managers are singletons you access via:
  - df::LogManager::getInstance()
  - df::GameManager::getInstance()
  - WorldManager& WM()  // convenience free function

Object lifetime:
  - Object base ctor: assigns unique id, sets type "Object", position (0,0), and **adds to world** automatically.
  - Object dtor: removes from world.
  - Do NOT call addToWorld() again in derived ctors (it would double-insert).

World storage:
  - WorldManager keeps two ObjectList instances:
      m_updates   – all live objects
      m_deletions – objects marked for deletion this frame
  - Call markForDelete() on an Object; WorldManager::update() removes and deletes after the current loop.

Events:
  - GameManager::run() creates EventStep(step_count) each iteration.
  - It iterates a **copy** of the world list (safe for deferred deletion), calls o->onEvent(evt), then WM().update().

Vector math:
  - Float-based Vector with normalize/scale/magnitude and operator+.

Logging:
  - LogManager opens "dragonfly.log" at startup and closes it at shutdown.
  - writeLog(const char* fmt, ...) mirrors printf formatting.
  - Optional: setFlush(true) in the test driver to see log lines appear immediately.


5) How to Build & Run (Visual Studio 2022)
------------------------------------------
1. Open the solution / project.
2. Ensure the app project (with DragonflyMattNickerson.cpp) is the **Startup Project**.
3. Build:
   - Ctrl+Shift+B  (or Build → Build Solution)
4. Run tests:
   - Ctrl+F5 (Run without debugging)

Where’s the logfile?
- Project Properties → Debugging → Working Directory (default: $(ProjectDir))
- Open that folder; you’ll see **dragonfly.log** after running.

If you turned on “Treat warnings as errors” (/WX) and see a C4996 for fopen:
- Either use the provided fopen_s path in LogManager.cpp, or add _CRT_SECURE_NO_WARNINGS before CRT includes.


6) How to Build & Run (Makefile, optional)
------------------------------------------
Requirements: g++/clang with C++17

From the repo root:
  make            # build into bin/dragonfly
  make run        # run the tests
  make clean      # remove build artifacts

Notes:
- The Makefile defines a small macro so fopen_s compiles under gcc/clang.


7) What the Tests Do (and how to read them)
-------------------------------------------
The test driver (DragonflyMattNickerson.cpp) logs a series of [PASS]/[FAIL] lines that cover:

Vector
  - default ctor, magnitude, normalize, scale, operator+, setXY

Clock
  - delta() timing (resets baseline), split() timing (non-reset)

Object & ObjectList
  - unique ids, type/position get/set
  - ObjectList insert/remove/clear/operator[]
  - add to world (handled by Object base ctor), markForDelete + update

WorldManager
  - getAllObjects(), objectsOfType("Dummy")
  - removeObject(), deferred deletions

GameManager
  - startup/shutdown messages
  - **game loop** sends EventStep each iteration
  - probe object ends the loop after N steps
  - “Game loop starting” / “Game loop ended” lines appear in the log

Expected result: all tests PASS. The final line shows a summary:
  == TEST SUMMARY: 26 passed, 0 failed ==


8) Known Choices / Assumptions
------------------------------
- Fixed timestep: This submission’s GameManager loop focuses on sending EventStep each iteration. A Clock class is provided and tested. If a fixed rate (e.g., 33ms) is required, the loop can be trivially adapted with Clock + Sleep; I kept it minimal per the Part A constraints and discussion.
- Object auto-insert: The base Object adds itself to the world. This keeps tests simple; derived ctors should NOT call addToWorld() again.
- ObjectList capacity: MAX_OBJECTS = 1000 (static array). Operator[] throws on out-of-range.
- Deletion model: WorldManager owns object deletion when they’re in the world and marked for delete. Objects removed from the world explicitly can be deleted by the owner (tests demonstrate both).
- No graphics/input: Per Part A, there is no rendering or input system. All output is via logging.


9) How to Grade Quickly
-----------------------
- Build & Run (VS2022 or make run)
- Open **dragonfly.log**
- Verify:
  - “WorldManager started”, “GameManager started”
  - PASS lines for Vector/Clock/Object/ObjectList/WorldManager
  - “Game loop starting/ended”
  - PASS summary at the end (0 failed)
- Spot-check:
  - Objects auto-add on construction (counts correct)
  - markForDelete + update removes objects
  - Step events reach objects each loop (StepProbe increments)

If anything fails, the log shows exactly which assertion to look at.

