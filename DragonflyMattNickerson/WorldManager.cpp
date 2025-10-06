#include "WorldManager.h"
#include "LogManager.h"
#include "Object.h"
#include <algorithm>

WorldManager::WorldManager() = default;

WorldManager& WorldManager::getInstance() {
    static WorldManager inst;
    return inst;
}

int WorldManager::startUp() {
    if (isStarted()) return 0;
    m_updates.clear();
    m_deletions.clear();
    df::Manager::startUp();
    df::LogManager::getInstance().writeLog("WorldManager started\n");
    return 0;
}

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

int WorldManager::insertObject(Object* p_o) {
    return m_updates.insert(p_o);
}

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

ObjectList WorldManager::getAllObjects() const {
    return m_updates; // return a copy per spec
}

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

int WorldManager::markForDelete(Object* p_o) {
    if (p_o == nullptr) return -1;
    // Prevent duplicates in deletions list.
    for (int i = 0; i < m_deletions.getCount(); ++i) {
        if (m_deletions[i] == p_o) return 0;
    }
    return m_deletions.insert(p_o);
}

WorldManager& WM() { return WorldManager::getInstance(); }