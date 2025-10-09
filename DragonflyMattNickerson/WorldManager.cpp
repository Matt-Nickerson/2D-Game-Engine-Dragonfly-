#include "WorldManager.h"
#include "LogManager.h"
#include "Object.h"
#include <algorithm>

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
    return m_updates; // return a copy per spec
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

// Update world. Delete Objects marked for deletion.
void WorldManager::update() {
    // Delete all Objects marked for deletion at end of loop.
    for (int i = 0; i < m_deletions.getCount(); ++i) {
        Object* o = m_deletions[i];
        if (o) {
            m_updates.remove(o);
            delete o; // engine owns lifetime in this iteration
        }
    }
    m_deletions.clear();
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

WorldManager& WM() { return WorldManager::getInstance(); }