#include "Vector.h"
#include <cmath>

// Constructor
Vector::Vector(float init_x, float init_y) : m_x(init_x), m_y(init_y) {}
Vector::Vector() : m_x(0.0f), m_y(0.0f) {}

// Get/set horizontal component.
void Vector::setX(float new_x) { m_x = new_x; }
float Vector::getX() const { return m_x; }

// Get/set vertical component.
void Vector::setY(float new_y) { m_y = new_y; }
float Vector::getY() const { return m_y; }

// Set horizontal & vertical components.
void Vector::setXY(float new_x, float new_y) { m_x = new_x; m_y = new_y; }

// Return magnitude of vector.
float Vector::getMagnitude() const {
	return std::sqrt(m_x * m_x + m_y * m_y);
}

// Normalize vector.
void Vector::normalize() {
	float len = getMagnitude();
	if (len > 0.0f) {
		m_x /= len;
		m_y /= len;
	}
}

// Scale vector.
void Vector::scale(float s) {
	m_x *= s;
	m_y *= s;
}

// Add two Vectors, return new Vector.
Vector Vector::operator+(const Vector& other) const {
	return Vector(m_x + other.m_x, m_y + other.m_y);
}