#pragma once
#include "Manager.h"
#include "ObjectList.h"
#include <string>


class WorldManager : public df::Manager {
private:
	WorldManager(); 
	WorldManager(WorldManager const&); // Don't allow copy.


	ObjectList m_updates; // All Objects in world to update.
	ObjectList m_deletions; // All Objects in world to delete.
	
	int m_width{ 80 };
	int m_height{ 24 };

	// Helpers
	bool withinBounds(const Vector& pos) const;
	ObjectList getCollisions(Object* mover, const Vector& where) const;
	bool moveObject(Object* p_o, const Vector& to); 

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

	// Set/get world boundary (default 80x24).
	void setBoundary(int width, int height);

	// Get world boundary.
	int  getBoundaryWidth() const { return m_width; }

	// Get world boundary.
	int  getBoundaryHeight() const { return m_height; }

	// Draw all objects to screen.
	void draw();
};


WorldManager& WM();