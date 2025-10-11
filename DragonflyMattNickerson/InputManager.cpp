#include "InputManager.h"
#include "WorldManager.h"
#include "LogManager.h"

#include "EventKeyboard.h"
#include "EventMouse.h"

#include <Windows.h>

using df::InputManager;

namespace {

    static const int kTrackedVKs[] = {
      VK_ESCAPE, VK_SPACE,
      'A','B','C','D','E','F','G','H','I','J','K','L','M',
      'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
      '0','1','2','3','4','5','6','7','8','9',
      VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
    };

    inline bool keyDown(int vk) {
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    }

    inline void dispatchToAll(const Event& e) {
        auto list = WM().getAllObjects();
        for (int i = 0; i < list.getCount(); ++i) {
            if (auto* o = list[i]) o->onEvent(e);
        }
    }

} 

namespace df {

    InputManager::InputManager() {
        setType("InputManager");
    }

    InputManager& InputManager::getInstance() {
        static InputManager inst;
        return inst;
    }

    int InputManager::startUp() {
        if (isStarted()) return 0;

        // Initialize previous states
        for (bool& b : m_prev_keys) b = false;
        m_prev_lb = m_prev_rb = m_prev_mb = false;

        POINT p{};
        if (GetCursorPos(&p)) {
            m_prev_x = p.x;
            m_prev_y = p.y;
        }
        else {
            m_prev_x = m_prev_y = 0;
        }

        Manager::startUp();
        LogManager::getInstance().writeLog("InputManager started\n");
        return 0;
    }

    void InputManager::shutDown() {
        LogManager::getInstance().writeLog("InputManager shutting down\n");
        Manager::shutDown();
    }
	// Get input from keyboard and mouse, generate events as needed.
    void InputManager::getInput() const {
        for (int vk : kTrackedVKs) {
            bool down = keyDown(vk);
            bool was = m_prev_keys[vk];

            if (down && !was) {
                // Key pressed
                EventKeyboard ek(vk);
                dispatchToAll(ek);
            }
            else if (!down && was) {
                EventKeyboard ek(-vk);
                dispatchToAll(ek);
            }
            m_prev_keys[vk] = down;
        }

        const bool lb = keyDown(VK_LBUTTON);
        const bool rb = keyDown(VK_RBUTTON);
        const bool mb = keyDown(VK_MBUTTON);

		// Mouse buttons
        if (lb != m_prev_lb) {
            EventMouse em(lb ? MouseAction::Pressed : MouseAction::Released,
                MouseButton::Left, m_prev_x, m_prev_y);
            dispatchToAll(em);
            m_prev_lb = lb;
        }
        if (rb != m_prev_rb) {
            EventMouse em(rb ? MouseAction::Pressed : MouseAction::Released,
                MouseButton::Right, m_prev_x, m_prev_y);
            dispatchToAll(em);
            m_prev_rb = rb;
        }
        if (mb != m_prev_mb) {
            EventMouse em(mb ? MouseAction::Pressed : MouseAction::Released,
                MouseButton::Middle, m_prev_x, m_prev_y);
            dispatchToAll(em);
            m_prev_mb = mb;
        }

        POINT p{};
        if (GetCursorPos(&p)) {
            if (p.x != m_prev_x || p.y != m_prev_y) {
                EventMouse move(MouseAction::Moved, MouseButton::None, static_cast<int>(p.x), static_cast<int>(p.y));
                dispatchToAll(move);
                m_prev_x = p.x;
                m_prev_y = p.y;
            }
        }
    }

} 
