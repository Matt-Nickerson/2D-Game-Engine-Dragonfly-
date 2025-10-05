#include "Object.h"
#include "WorldManager.h"


int Object::s_next_id = 1;


Object::Object() : m_id(s_next_id++) {}
Object::~Object() = default;


int Object::onEvent(const Event&) { return 0; }


void Object::addToWorld() { WorldManager().addObject(this); }
void Object::removeFromWorld() { WorldManager().removeObject(this); }
void Object::markForDelete() { m_marked = true; WorldManager().markForDelete(this); }