#pragma once
#include <string>
#include "Vector.h"


class Event;  

enum class Solidness {
	HARD,     
	SOFT,     
	SPECTRAL
};

class Object {
private:
	int m_id; // Unique game engine defined identifier.
	std::string m_type; // Game programmer defined type.
	Vector m_position; // Position in game world.
	bool m_marked = false; // For deferred deletion (engine convenience).

	Solidness   m_solidness{ Solidness::HARD };
	int         m_altitude{ 0 };        
	float       m_vx{ 0.0f };
	float       m_vy{ 0.0f };

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

	virtual int onEvent(const Event& e);

	void        setSolidness(Solidness s);
	Solidness   getSolidness() const;
	bool        isSolid() const;  

	void        setAltitude(int a);
	int         getAltitude() const;

	void        setVelocityX(float vx);
	void        setVelocityY(float vy);
	void        setVelocity(float vx, float vy);
	float       getVelocityX() const;
	float       getVelocityY() const;


	// Mark for deletion via WorldManager deferred removal.
	void markForDelete();
	bool isMarkedForDelete() const { return m_marked; }

	virtual int draw() { return 0;}
};