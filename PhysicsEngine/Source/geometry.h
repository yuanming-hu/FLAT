#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <cassert>
#include "constants.h"
#define hypot _hypot

inline int sgn(double x) {
	if (x < -eps) return -1;
	if (x > eps) return 1;
	return 0;
}

struct Vector2D {
	double x, y, z;
	Vector2D() {}
	Vector2D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	Vector2D operator += (const Vector2D &vec) {
		return *this = Vector2D(x + vec.x, y + vec.y, z + vec.z);
	}
	Vector2D operator -= (const Vector2D &vec) {
		return *this = Vector2D(x - vec.x, y - vec.y, z - vec.z);
	}
	Vector2D operator *= (double a) {
		return *this = Vector2D(x * a, y * a, z);
	}
	Vector2D operator /= (double a) {
		return *this = Vector2D(x / a, y / a, z);
	}
	Vector2D operator / (double a) const {
		return Vector2D(x / a, y / a, z);
	}
	Vector2D operator +(Vector2D vec) const {
		return Vector2D(x + vec.x, y + vec.y, z + vec.z);
	}
	Vector2D operator -(Vector2D vec) const {
		return Vector2D(x - vec.x, y - vec.y, z - vec.z);
	}
	Vector2D operator -() const{
		return Vector2D(-x, -y, z);
	}
	double operator *(Vector2D vec) const {
		return x * vec.x + y * vec.y;
	}
	double operator %(Vector2D vec) const {
		return x * vec.y - y * vec.x;
	}
	void Normalize() {
		double r = 1. / hypot(x, y);
		x *= r, y *= r;
	}
	Vector2D GetDirection() const {
		double r = 1. / hypot(x, y);
		return Vector2D(x * r, y * r, 0);
	}
	double GetLength() const {
		return hypot(x, y);
	}
	double GetLength2() const {
		return x * x + y * y;
	}
	Vector2D Rotate(double angle) const {
		return Vector2D(x * cos(angle) - y * sin(angle), y * cos(angle) + x * sin(angle), z);
	}
	Vector2D Rotate() const {
		return Vector2D(-y, x, z);
	}
	Vector2D InverseRotate() const {
		return Vector2D(y, -x, z);
	}
};

inline Vector2D operator *(double a, const Vector2D &vec) {
	return Vector2D(a * vec.x, a * vec.y, vec.z);	
}

inline Vector2D operator *(const Vector2D &vec, double a) {
	return Vector2D(a * vec.x, a * vec.y, vec.z);	
}

class Line {
	Vector2D a, b, v;
public:
	Line() {}
	Line(Vector2D _a, Vector2D _b) : a(_a), b(_b), v(_b - _a) {}
	Vector2D GetA() const {return a;}
	Vector2D GetB() const {return b;}
	Vector2D GetV() const {return v;}
	bool IsParallel(Line l) const {
		return abs(l.v % v) < eps;
	}
	bool IsInside(Vector2D p) const {
		return sgn((p - a) * v) * sgn((p - b) * v) < eps;
	}
};

inline Vector2D operator *(const Line &a, const Line &b) {
	return b.GetA() + (((a.GetA() - b.GetA()) % a.GetV()) / (b.GetV() % a.GetV())) * b.GetV();
}


struct Matrix3x3 {
	double a[3][3];
	double *operator[] (int i) {return a[i];}
	const double *operator[] (int i) const {return a[i];}
	Matrix3x3() {memset(a, 0, sizeof(a));}
	Matrix3x3 Transpose() {
		Matrix3x3 res = *this;
		swap(res[0][1], res[1][0]);
		swap(res[0][2], res[2][0]);
		swap(res[1][2], res[2][1]);
		return res;
	}
	Vector2D operator() (Vector2D vec) const {
		return Vector2D(vec.x * a[0][0] + vec.y * a[0][1] + vec.z * a[0][2],
						vec.x * a[1][0] + vec.y * a[1][1] + vec.z * a[1][2],
						vec.x * a[2][0] + vec.y * a[2][1] + vec.z * a[2][2]);
	}
	void SetAsAxisTransform(Vector2D movement, double rotationAngle) {
		a[0][0] = cos(rotationAngle); a[0][1] = -sin(rotationAngle);
		a[1][0] = -a[0][1], a[1][1] = a[0][0];
		a[0][2] = movement.x; a[1][2] = movement.y;
		a[2][0] = a[2][1] = 0; a[2][2] = 1;
	}
	void SetAsAxisTransformInverse(Vector2D movement, double rotationAngle) {
		double cosV = cos(rotationAngle), sinV = sin(rotationAngle);
		a[0][0] = a[1][1] = cosV;
		a[0][1] = sinV; a[1][0] = -sinV;
		a[2][0] = a[2][1] = 0; a[2][2] = 1;
		a[0][2] = -movement.x * cosV - movement.y * sinV;
		a[1][2] = movement.x * sinV - movement.y * cosV;
	}
};

inline Matrix3x3 operator *(const Matrix3x3 &a, const Matrix3x3 &b) {
	Matrix3x3 c;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++) 
				c[i][j] += a[i][k] * b[k][j];
	return c;
} 


#endif
