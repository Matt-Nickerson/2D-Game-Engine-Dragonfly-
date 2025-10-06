#include "Object.h"
#include "WorldManager.h"


int Object::s_next_id = 0;


Object::Object()
	: m_id(s_next_id++), m_type("Object"), m_position(0.0f, 0.0f) {
	addToWorld();
}


Object::~Object() {
	removeFromWorld();
}


void Object::setId(int new_id) { m_id = new_id; }
int Object::getId() const { return m_id; }


void Object::setType(std::string new_type) { m_type = std::move(new_type); }
std::string Object::getType() const { return m_type; }


void Object::setPosition(Vector new_pos) { m_position = new_pos; }
Vector Object::getPosition() const { return m_position; }


void Object::addToWorld() { WM().insertObject(this); }
void Object::removeFromWorld() { WM().removeObject(this); }


void Object::markForDelete() { m_marked = true; WM().markForDelete(this); }