#include "Object.h"
#include "WorldManager.h"


int Object::s_next_id = 0;

// Create Object with default values and add to WorldManager.
Object::Object()
	: m_id(s_next_id++), m_type("Object"), m_position(0.0f, 0.0f) {
	addToWorld();
}

// Remove Object from WorldManager.
Object::~Object() {
	removeFromWorld();
}

// Set and get id.
void Object::setId(int new_id) { m_id = new_id; }
int Object::getId() const { return m_id; }

// Set and get type.
void Object::setType(std::string new_type) { m_type = std::move(new_type); }
std::string Object::getType() const { return m_type; }

// Set and get position.
void Object::setPosition(Vector new_pos) { m_position = new_pos; }
Vector Object::getPosition() const { return m_position; }

// Add/remove self to/from world.
void Object::addToWorld() { WM().insertObject(this); }
void Object::removeFromWorld() { WM().removeObject(this); }

// Mark for deletion via WorldManager deferred removal.
void Object::markForDelete() { m_marked = true; WM().markForDelete(this); }