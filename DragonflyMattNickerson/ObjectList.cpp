#include "ObjectList.h"


ObjectList::ObjectList() : m_count(0) {}


void ObjectList::clear() { m_count = 0; }


int ObjectList::getCount() const { return m_count; }


int ObjectList::insert(Object* p_o) {
	if (m_count >= MAX_OBJECTS || p_o == nullptr) return -1;
	m_p_obj[m_count++] = p_o;
	return 0;
}


int ObjectList::remove(Object* p_o) {
	if (m_count <= 0 || p_o == nullptr) return -1;
	for (int i = 0; i < m_count; ++i) {
		if (m_p_obj[i] == p_o) {
			// Scoot tail over one.
			for (int j = i; j < m_count - 1; ++j) {
				m_p_obj[j] = m_p_obj[j + 1];
			}
			--m_count;
			return 0; // Found.
		}
	}
	return -1; // Not found.
}


Object* ObjectList::operator[](int i) {
	if (i < 0 || i >= m_count) throw std::out_of_range("ObjectList index out of range");
	return m_p_obj[i];
}
