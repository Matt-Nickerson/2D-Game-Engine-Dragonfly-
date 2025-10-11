#include "ObjectList.h"

// Default constructor
ObjectList::ObjectList() : m_count(0) {}

// Clear list (setting count to 0).
void ObjectList::clear() {
	for (int i = 0; i < m_count; ++i) m_p_obj[i] = nullptr;
	m_count = 0;
}
// Remove object pointer from list. Return 0 if found, else -1.
int ObjectList::remove(Object* p_o) {
	if (m_count <= 0 || p_o == nullptr) return -1;
	for (int i = 0; i < m_count; ++i) {
		if (m_p_obj[i] == p_o) {
			for (int j = i; j < m_count - 1; ++j)
				m_p_obj[j] = m_p_obj[j + 1];
			--m_count;
			m_p_obj[m_count] = nullptr;      // clear vacated tail slot
			return 0;
		}
	}
	return -1;
}

// Return count of number of objects in list.
int ObjectList::getCount() const { return m_count; }

// Insert object pointer in list. Return 0 if ok, else -1.
int ObjectList::insert(Object* p_o) {
	if (m_count >= MAX_OBJECTS || p_o == nullptr) return -1;
	m_p_obj[m_count++] = p_o;
	return 0;
}

// Index operator (with bounds checking). Returns pointer to Object.
Object* ObjectList::operator[](int i) {
	if (i < 0 || i >= m_count) throw std::out_of_range("ObjectList index out of range");
	return m_p_obj[i];
}
const Object* ObjectList::operator[](int i) const {
	if (i < 0 || i >= m_count)
		throw std::out_of_range("ObjectList index out of range");
	return m_p_obj[i];
}
