#include "Object.h"
#include "WorldManager.h"


int Object::s_next_id = 0;

int Object::onEvent(const Event& e) {
	(void)e;
	return 0;
}
// Create Object with default values and add to WorldManager.
Object::Object()
    : m_id(s_next_id++),
    m_type("Object"),
    m_position(0.f, 0.f),
    m_marked(false),
    m_solidness(Solidness::HARD),
    m_altitude(0),
    m_vx(0.0f),
    m_vy(0.0f) {
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

void Object::setSolidness(Solidness s) { m_solidness = s; }
Solidness Object::getSolidness() const { return m_solidness; }
bool Object::isSolid() const { return m_solidness != Solidness::SPECTRAL; }

void Object::setAltitude(int a) { m_altitude = a; }
int Object::getAltitude() const { return m_altitude; }

void Object::setVelocityX(float vx) { m_vx = vx; }
void Object::setVelocityY(float vy) { m_vy = vy; }
void Object::setVelocity(float vx, float vy) { m_vx = vx; m_vy = vy; }
float Object::getVelocityX() const { return m_vx; }
float Object::getVelocityY() const { return m_vy; }