#pragma once
#include "Manager.h"
#include "ObjectList.h"
#include <string>


// Listing 4.57 style WorldManager
class WorldManager : public df::Manager {
private:
	WorldManager(); // Private (a singleton).
	WorldManager(WorldManager const&); // Don't allow copy.


	ObjectList m_updates; // All Objects in world to update.
	ObjectList m_deletions; // All Objects in world to delete.


public:
	// Get the one and only instance of the WorldManager.
	static WorldManager& getInstance();


	// Startup game world (initialize everything to empty). Return 0.
	int startUp() override;


	// Shutdown game world (delete all game world Objects).
	void shutDown() override;


	// Insert Object into world. Return 0 if ok, else -1.
	int insertObject(class Object* p_o);


	// Remove Object from world. Return 0 if ok, else -1.
	int removeObject(class Object* p_o);


	// Return list of all Objects in world.
	ObjectList getAllObjects() const;


	// Return list of all Objects in world matching type.
	ObjectList objectsOfType(std::string type);


	// Update world. Delete Objects marked for deletion.
	void update();


	// Indicate Object is to be deleted at end of current game loop. Return 0 if ok, else -1.
	int markForDelete(class Object* p_o);
};


WorldManager& WM();