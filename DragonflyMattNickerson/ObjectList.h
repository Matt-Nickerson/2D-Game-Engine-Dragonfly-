#include "Object.h"
#include <stdexcept>


const int MAX_OBJECTS = 1000;


class ObjectList {
private:
	int m_count; // Count of objects in list.
	Object* m_p_obj[MAX_OBJECTS]; // Array of pointers to objects.


public:
	// Default constructor.
	ObjectList();


	// Insert object pointer in list. Return 0 if ok, else -1.
	int insert(Object* p_o);


	// Remove object pointer from list. Return 0 if found, else -1.
	int remove(Object* p_o);


	// Clear list (setting count to 0).
	void clear();


	// Return count of number of objects in list.
	int getCount() const;


	// Index operator (with bounds checking). Returns pointer to Object.
	Object* operator[](int i);
};