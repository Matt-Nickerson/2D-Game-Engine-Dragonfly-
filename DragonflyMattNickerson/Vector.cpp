#include "Vector.h"


Vector::Vector() = default;


Vector::Vector(int x, int y) : m_x(x), m_y(y) {}


int Vector::getX() const { return m_x; }
int Vector::getY() const { return m_y; }


void Vector::setX(int x) { m_x = x; }
void Vector::setY(int y) { m_y = y; }
void Vector::setXY(int x, int y) { m_x = x; m_y = y; }
Vector Vector::addXY(const Vector& x, const Vector& y) {
    return Vector(x.getX() + y.getX(), x.getY() + y.getY());
}
