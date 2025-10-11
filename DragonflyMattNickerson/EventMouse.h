#pragma once
#include "Event.h"
#include <string>

// High-level mouse actions and buttons
enum class MouseAction { Moved, Pressed, Released };
enum class MouseButton { None, Left, Right, Middle };

// Mouse event: action/button plus screen coordinates.
class EventMouse : public Event {
    MouseAction m_action{ MouseAction::Moved };
    MouseButton m_button{ MouseButton::None };
    int         m_x{ 0 }; // screen X
    int         m_y{ 0 };// screen Y

public:
    static const std::string TYPE; // "mouse"

    EventMouse() : Event(TYPE) {}
    EventMouse(MouseAction act, MouseButton btn, int x, int y)
        : Event(TYPE), m_action(act), m_button(btn), m_x(x), m_y(y) {
    }

    // Action
    void        setAction(MouseAction a) { m_action = a; }
    MouseAction getAction() const { return m_action; }

    // Button
    void        setButton(MouseButton b) { m_button = b; }
    MouseButton getButton() const { return m_button; }

    // Screen coordinates
    void setX(int x) { m_x = x; }
    void setY(int y) { m_y = y; }
    void setXY(int x, int y) { m_x = x; m_y = y; }
    int  getX() const { return m_x; }
    int  getY() const { return m_y; }
};
