#include <string>
#include "Vector.h"
#include "Event.h"

class Object {
	static int s_next_id;
	int m_id;
	std::string m_type;
	Vector m_pos;
	bool m_marked = false;
public:
	Object();
	virtual ~Object();


	int getId() const { return m_id; }
	const std::string& getType() const { return m_type; }
	void setType(const std::string& t) { m_type = t; }


	const Vector& getPosition() const { return m_pos; }
	void setPosition(const Vector& v) { m_pos = v; }


	virtual int onEvent(const Event& e); // return 1 if handled


	void addToWorld();
	void removeFromWorld();


	void markForDelete();
	bool isMarkedForDelete() const { return m_marked; }
};