#pragma once
#include "Event.h"
#include "Vector.h"
#include <string>

class Object;

// Carries collision info between two objects at a world position.
class EventCollision : public Event {
    Object* m_p_obj1{ nullptr };
    Object* m_p_obj2{ nullptr };
    Vector  m_pos; // collision position in world coords

public:
    static const std::string TYPE; // "collision"

    EventCollision() : Event(TYPE) {}
    EventCollision(Object* a, Object* b, const Vector& where)
        : Event(TYPE), m_p_obj1(a), m_p_obj2(b), m_pos(where) {
    }

    // Get/set colliders
    void    setObject1(Object* a) { m_p_obj1 = a; }
    void    setObject2(Object* b) { m_p_obj2 = b; }
    Object* getObject1() const { return m_p_obj1; }
    Object* getObject2() const { return m_p_obj2; }

    // Get/set collision position
    void    setPosition(const Vector& v) { m_pos = v; }
    Vector  getPosition() const { return m_pos; }
};