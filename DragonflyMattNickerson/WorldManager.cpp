#include "WorldManager.h"
#include "LogManager.h"
#include "Object.h"
#include <algorithm>
#include "EventOut.h"
#include "EventCollision.h"
#include <iostream>
#include "Vector.h"
#include "Manager.h"
#include <vector>


// Singleton
WorldManager::WorldManager() = default;

// Get the one and only instance of the WorldManager.
WorldManager& WorldManager::getInstance() {
    static WorldManager inst;
    return inst;
}
// Startup game world (initialize everything to empty). Return 0.
int WorldManager::startUp() {
    if (isStarted()) return 0;
    m_updates.clear();
    m_deletions.clear();
    df::Manager::startUp();
    df::LogManager::getInstance().writeLog("WorldManager started\n");
    m_width = 80;
    m_height = 24;
    return 0;
}
// Shutdown game world (delete all game world Objects).
void WorldManager::shutDown() {
    df::LogManager::getInstance().writeLog("WorldManager shutting down\n");
    // Delete remaining objects to avoid leaks.
    for (int i = 0; i < m_updates.getCount(); ++i) {
        delete m_updates[i];
    }
    m_updates.clear();
    m_deletions.clear();
    df::Manager::shutDown();
}
// Insert Object into world. Return 0 if ok, else -1.
int WorldManager::insertObject(Object* p_o) {
    return m_updates.insert(p_o);
}
void WorldManager::setBoundary(int width, int height) {
    // keep sane values
    if (width < 1)  width = 1;
    if (height < 1)  height = 1;

    m_width = width;
    m_height = height;

    df::LogManager::getInstance().writeLog(
        "WorldManager: boundary set to %dx%d\n", m_width, m_height);
}

// Remove Object from world. Return 0 if ok, else -1.
int WorldManager::removeObject(Object* p_o) {
    // If scheduled for deletion, drop it from deletions too.
    for (int i = 0; i < m_deletions.getCount(); ++i) {
        if (m_deletions[i] == p_o) {
            m_deletions.remove(p_o);
            break;
        }
    }
    return m_updates.remove(p_o);
}

// Return list of all Objects in world.
ObjectList WorldManager::getAllObjects() const {
    return m_updates; 
}

// Return list of all Objects in world matching type.
ObjectList WorldManager::objectsOfType(std::string type) {
    ObjectList list;
    for (int i = 0; i < m_updates.getCount(); ++i) {
        Object* o = m_updates[i];
        if (o && o->getType() == type) {
            list.insert(o);
        }
    }
    return list;
}

// Indicate Object is to be deleted at end of current game loop. Return 0 if ok, else -1.
int WorldManager::markForDelete(Object* p_o) {
    if (p_o == nullptr) return -1;
    // Prevent duplicates in deletions list.
    for (int i = 0; i < m_deletions.getCount(); ++i) {
        if (m_deletions[i] == p_o) return 0;
    }
    return m_deletions.insert(p_o);
}

bool WorldManager::withinBounds(const Vector& pos) const {
    const int x = static_cast<int>(pos.getX());
    const int y = static_cast<int>(pos.getY());
    return (x >= 0 && x < m_width && y >= 0 && y < m_height);
}

ObjectList WorldManager::getCollisions(Object* mover, const Vector& where) const {
    ObjectList hits;
    for (int i = 0; i < m_updates.getCount(); ++i) {
        const Object* other = m_updates[i]; // only one declaration
        if (!other || other == mover) continue;

        if (static_cast<int>(other->getPosition().getX()) == static_cast<int>(where.getX()) &&
            static_cast<int>(other->getPosition().getY()) == static_cast<int>(where.getY())) {
            hits.insert(const_cast<Object*>(other));
        }
    }
    return hits;
}


bool WorldManager::moveObject(Object* p_o, const Vector& to) {
    if (!p_o) return false;

    // 1) Out-of-bounds?
    if (!withinBounds(to)) {
        EventOut out;
        p_o->onEvent(out);
        return false; // block movement when leaving the world
    }

    // 2) Collisions at destination
    ObjectList hits = getCollisions(p_o, to);

    // If any hit is solid vs solid, block and emit collisions to both
    bool blocked = false;
    for (int i = 0; i < hits.getCount(); ++i) {
        Object* other = hits[i];
        if (!other) continue;

        const bool mover_solid = p_o->getSolidness() != Solidness::SPECTRAL;
        const bool other_solid = other->getSolidness() != Solidness::SPECTRAL;

        if (mover_solid && other_solid) {
            EventCollision col(p_o, other, to);
            p_o->onEvent(col);
            other->onEvent(col);
            blocked = true;
            // note: keep checking others to send all collision events this step
        }
        else {
            // At least one spectral -> allow pass-through but still send event
            EventCollision col(p_o, other, to);
            p_o->onEvent(col);
            other->onEvent(col);
            // do not mark blocked
        }
    }

    if (blocked) return false;

    p_o->setPosition(to);
    return true;
}

// Update world. Move objects according to their velocity.
void WorldManager::update() {
    for (int i = 0; i < m_updates.getCount(); ++i) {
        Object* o = m_updates[i];
        if (!o) continue;

        const float vx = o->getVelocityX();
        const float vy = o->getVelocityY();
        if (vx == 0.0f && vy == 0.0f) continue;

        Vector from = o->getPosition();
        Vector to(from.getX() + vx, from.getY() + vy);

        (void)moveObject(o, to);
    }

    for (int i = 0; i < m_deletions.getCount(); ++i) {
        if (Object* o = m_deletions[i]) {
            m_updates.remove(o);
            delete o;
        }
    }
    m_deletions.clear();
}


// Draw all objects to screen.
void WorldManager::draw() {
    std::vector<Object*> order;
    order.reserve(static_cast<size_t>(m_updates.getCount()));
    for (int i = 0; i < m_updates.getCount(); ++i) {
        if (auto* o = m_updates[i]) order.push_back(o);
    }

    std::sort(order.begin(), order.end(),
        [](const Object* a, const Object* b) {
            if (a->getAltitude() != b->getAltitude())
                return a->getAltitude() < b->getAltitude();  
            return a->getId() < b->getId();                  
        });

    for (auto* o : order) {
        if (o) (void)o->draw();
    }
}


WorldManager& WM() { return WorldManager::getInstance(); }