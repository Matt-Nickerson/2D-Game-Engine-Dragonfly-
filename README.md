# Dragonfly — Part A/B (Matt Nickerson)

**Student:** Matthew Nickerson  
**Login:** mwnickerson@wpi.edu  
**Course:** IMGD 3000  
**Date:** 2025-10-10

---

## Platform

- **Primary:** Windows 10/11 (x64), Visual Studio 2022 (**C++17**)
- **Optional cross-build:** Makefile (g++/clang, C++17)

## Dependencies

- **Part A only:** none (logging only; no graphics/input required).
- **Display/Input (optional / Part B+):** **SFML 2.6.x or 3.x**
  - **vcpkg (recommended):**
    ```powershell
    vcpkg install sfml:x64-windows
    ```
  - **Manual:** download SFML SDK, set *Include Directories* and *Library Directories*, and link:
    - `sfml-graphics`, `sfml-window`, `sfml-system` (use `-d` variants for Debug builds)
  - Place a monospace font (e.g., `df-font.ttf`) next to the executable.

---

## What’s Implemented

### Managers / lifecycle

- **Manager (base):** startup/shutdown, type id, `isStarted()`.
- **LogManager (singleton):** open/close logfile `dragonfly.log`, printf-style `writeLog()`, optional `setFlush(true)`.
- **GameManager (singleton):** startup/shutdown; **game loop** that each frame:
  - Sends **EventStep** to all objects.
  - (If enabled) calls **InputManager::getInput()**, **WorldManager::draw()**, **DisplayManager::swapBuffers()**.
- **WorldManager (singleton):**
  - Stores all game **Objects**
  - **Add/remove** objects; `getAllObjects()`, `objectsOfType()`
  - **Deferred deletion** via `markForDelete()` + `update()`
  - **Movement**, **collision detection**, **out-of-bounds** events
  - **Draw** in **ascending altitude**
  - **Boundary** set/get (default 80×24)

### Core data & types

- **Vector:** 2D float vector; `get/set`, `setXY`, `getMagnitude`, `normalize`, `scale`, `operator+`.
- **Object (base):**
  - Unique **ID**, **type** string, **position** (Vector)
  - Adds itself to world in base ctor, removes in dtor
  - **Solidness:** `HARD`, `SOFT`, `SPECTRAL`
  - **Altitude:** integer for draw ordering
  - **Velocity:** `vx`, `vy`
  - Hooks: `virtual int onEvent(const Event&)`, `virtual int draw()`
- **ObjectList:** fixed-capacity array of `Object*`; `insert/remove/clear/getCount`; `operator[]` (const + non-const) with range checks.

### Events

- **Event (base):** type string getter/setter
- **EventStep:** step count
- **EventOut:** mover tried to leave world bounds
- **EventCollision:** both objects + collision position
- **EventMouse:** button pressed/released + screen location
- **EventKeyboard:** key pressed (Windows VK_*)

### Input & Display (optional / SFML)

- **InputManager (singleton):** startup/shutdown; polls **keyboard & mouse**; dispatches **EventKeyboard**/**EventMouse**.
- **DisplayManager (singleton):** startup/shutdown; **drawCh** and **drawString** at grid (x,y) with optional color & justification; **swapBuffers()**; reports pixel/char bounds.
  - Defaults: **1024×768 px**, **80×24** cells, title “Dragonfly”, font `df-font.ttf`.

---

## Repository Layout

```
include/
  Manager.h       LogManager.h      WorldManager.h   GameManager.h
  Vector.h        Object.h          ObjectList.h     Event.h
  EventStep.h     EventOut.h        EventCollision.h
  EventMouse.h    EventKeyboard.h   Clock.h
  DisplayManager.h  InputManager.h   Color.h

src/
  Manager.cpp     LogManager.cpp    WorldManager.cpp GameManager.cpp
  Vector.cpp      Object.cpp        ObjectList.cpp   EventStep.cpp
  EventOut.cpp    EventCollision.cpp EventMouse.cpp  EventKeyboard.cpp
  Clock.cpp
  DisplayManager.cpp  InputManager.cpp

DragonflyMattNickerson.cpp   # test driver (main)
Makefile
README.md (this file)
dragonfly.log (created at runtime)
df-font.ttf (font for DisplayManager)
```

---

## Code Structure & Notes

- **Singleton access**
  ```cpp
  df::LogManager::getInstance();
  df::GameManager::getInstance();
  WorldManager& WM(); // free convenience function
  ```
- **Object lifetime**
  - Base ctor auto-adds to world; dtor removes from world.
  - Do **not** double-insert in derived ctors.
- **Deletion model**
  - Call `markForDelete()`. `WorldManager::update()` removes & deletes at frame end.
- **Movement & collisions**
  - `WorldManager::update()` advances by velocity via `moveObject()`.
  - **SPECTRAL** passes through (still receives **EventCollision**).
  - **HARD/SOFT** colliding with non-spectral **blocks** movement; both get **EventCollision**.
  - Out-of-bounds → **EventOut** and movement blocked.
- **Draw order**
  - Ascending **altitude** (lowest first). Ties broken deterministically (e.g., by id).

---

## Build & Run (Visual Studio 2022)

1. Open the solution/project.
2. Ensure **C++17**: *Project → C/C++ → Language → C++ Language Standard: ISO C++17*.
3. If using Display/Input:
   - Install SFML (vcpkg or manual link).
   - Place `df-font.ttf` in the working directory.
4. **Build:** `Ctrl+Shift+B`
5. **Run tests:** `Ctrl+F5`

**Logfile location:** working directory (see *Project → Properties → Debugging → Working Directory*). The file is `dragonfly.log`.

---

## Build & Run (Makefile, optional)

Requirements: g++/clang with C++17.


## Tests (what they cover)

The `DragonflyMattNickerson.cpp` test driver logs `[PASS]/[FAIL]` lines for:

- **Vector:** default, magnitude, normalize, scale, `operator+`, `setXY`
- **Clock:** `delta()` timing (resets), `split()` timing (non-reset)
- **Object & ObjectList:**
  - unique ids, type/position get/set
  - list: insert/remove/clear/operator[]
  - auto add (ctor), `markForDelete` + `update`
- **WorldManager:**
  - clear world, insert counts, `objectsOfType`
  - explicit remove, deferred deletion
  - **movement**, **out-of-bounds**, **collisions**
  - **draw order by altitude**
- **GameManager:**
  - loop sends **EventStep** each iteration (probe object ends loop after N steps)
- **Display/Input (smoke):**
  - draw two frames of text and swap
  - (optional manual) press a key/click to see input events

**Expected:** all tests PASS; ending with:
```
== TEST SUMMARY: XX passed, 0 failed ==
```

---

## Known Choices / Assumptions

- **Fixed timestep:** `Clock` + Windows `Sleep()` support a target frame time; loop uses microsecond timing and oversleep adjustment. Tests emphasize correctness of step events and world updates.
- **Altitude convention:** lower altitude renders first.
- **ObjectList capacity:** `MAX_OBJECTS = 1000`.
- **Color:** simple RGBA helper mapped to `sf::Color`.
- **Input VK codes:** Windows `VK_*`.

---

## Quick Grading Checklist

1. Build & Run (VS2022 or `make run`).
2. Open `dragonfly.log`.
3. Verify:
   - “WorldManager started”, “GameManager started”
   - PASS lines for Vector/Clock/Object/ObjectList/World
   - “Game loop starting/ended”
   - Draw-order test passes
4. Final summary: `== TEST SUMMARY: … passed, 0 failed ==`

If anything fails, the log’s assertion label points to the exact subsystem to inspect.
