#include <string>


// Engine includes
#include "Vector.h"


class Object {
private:
	int m_id; // Unique game engine defined identifier.
	std::string m_type; // Game programmer defined type.
	Vector m_position; // Position in game world.
	bool m_marked = false; // For deferred deletion (engine convenience).


	static int s_next_id; // static counter for unique ids


public:
	// Construct Object. Set default parameters and add to game world (WorldManager).
	Object();


	// Destroy Object. Remove from game world (WorldManager).
	virtual ~Object();


	// Set Object id.
	void setId(int new_id);


	// Get Object id.
	int getId() const;


	// Set type identifier of Object.
	void setType(std::string new_type);


	// Get type identifier of Object.
	std::string getType() const;


	// Set position of Object.
	void setPosition(Vector new_pos);


	// Get position of Object.
	Vector getPosition() const;


	// Add/remove self to/from world explicitly (helper methods).
	void addToWorld();
	void removeFromWorld();


	// Mark for deletion via WorldManager deferred removal.
	void markForDelete();
	bool isMarkedForDelete() const { return m_marked; }
};