class Vector {
	int m_x{ 0 };
	int m_y{ 0 };
public:
	Vector() = default;
	Vector(int x, int y) : m_x(x), m_y(y) {}

	int getX() const { return m_x; }
	int getY() const { return m_y; }
	void setX(int x) { m_x = x; }
	void setY(int y) { m_y = y; }
	void setXY(int x, int y) { m_x = x; m_y = y; }
	Vector addXY(const Vector& x, const Vector& y);
};