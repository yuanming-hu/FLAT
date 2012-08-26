#ifndef POLYGON_H
#define POLYGON_H

#include "graphics.h"
#include "geometry.h"

class Shape : public ScreenObject {
protected:
	bool fixed;
	Vector2D centerPosition, centerVelocity;
	double rotationAngle, angularVelocity;
	Matrix3x3 transformToWorld, transformToWorldInverse;
	double mass;
	double momentOfInertia;
	double circleRadius;
	double density;
	void ResetTransformToWorld() {
		transformToWorld.SetAsAxisTransform(centerPosition, rotationAngle);
		transformToWorldInverse.SetAsAxisTransformInverse(centerPosition, rotationAngle);
	}
public:
	void SetDensity(double density) {
		mass = mass / this->density * density;
		momentOfInertia = momentOfInertia / this->density * density;
		this->density = density;
	}
	void SetCenterPotision(Vector2D _centerPosition) {
		centerPosition = _centerPosition;
		ResetTransformToWorld();
	}
	void SetRotationAngle(double _rotationAngle) {
		rotationAngle = _rotationAngle;
		ResetTransformToWorld();
	}
	void SetFixed(bool _fixed) {
		fixed = _fixed;
	}
	void AddImpluse(Vector2D r, Vector2D p, bool paintNeed = false) {
		if (fixed) return;
		centerVelocity += p / mass;
		angularVelocity += (r - centerPosition) % p / momentOfInertia;
//		if (paintNeed)
//			pGraphics->DrawLine((int)r.x, (int)r.y, (int)(r.x + p.x / 100), (int)(r.y + p.y / 100), RGB(255, 0, 0));
	}
	void SetMass(double _mass) {
		mass = _mass;
	}
	void SetMomentOfInertia(double _momentOfInertia) {
		momentOfInertia = _momentOfInertia;
	}
	void Proceed(double T) {
		if (fixed) return; 
		centerPosition += T * centerVelocity;
		rotationAngle += T * angularVelocity;
		ResetTransformToWorld();
	}
	Vector2D PointVelocity(Vector2D p) {
		return centerVelocity + Vector2D(-angularVelocity * (p.y - centerPosition.y), angularVelocity * (p.x - centerPosition.x), 0);
	}
	
};

class Polygon : public Shape {
	friend class Physics;
	friend class Contact;
	friend class Constraint;
	friend class DistanceConstraint;
private:
	static const int constMaxnPoints = 32;
	int nPoints;
	Vector2D points[constMaxnPoints];
	double GetCutLengthX(double x) {
		double minI = -1e300, maxI = 1e300;
		Line cutLine(Vector2D(x, 0, 1), Vector2D(x, 1, 1));
		for (int i = 0; i < nPoints; i++) {
			Line edge(points[i], points[(i + 1) % nPoints]);
			if (sgn(edge.GetV().x) == 0) continue;
			else {
				double c = (edge * cutLine).y;
				if (sgn(edge.GetV().x) < 0) {
					maxI = min(maxI, c);
				} else {
					minI = max(minI, c);					
				}
			}
		}
		return maxI - minI;
	}
	double GetCutLengthY(double y) {
		double minI = -1e300, maxI = 1e300;
		Line cutLine(Vector2D(0, y, 1), Vector2D(1, y, 1));
		for (int i = 0; i < nPoints; i++) {
			Line edge(points[i], points[(i + 1) % nPoints]);
			if (sgn(edge.GetV().y) == 0) continue;
			else {
				double c = (edge * cutLine).x;
				if (sgn(edge.GetV().y) > 0) {
					maxI = min(maxI, c);
				} else {
					minI = max(minI, c);					
				}
			}
		}
		return maxI - minI;
	}
	

public:
	Polygon() {
		fixed = false;
		nPoints = 0;
		pGraphics = 0;
		density = 1.;
		rotationAngle = 0;
		angularVelocity = 0;
		centerVelocity = Vector2D(0, 0, 0);
	}
	void AddPoint(Vector2D point) {
		points[nPoints++] = point;
	}
	void Redraw() {
		static Vector2D worldPoints[constMaxnPoints];
		for (int i = 0; i < nPoints; i++) 
			worldPoints[i] = transformToWorld(points[i]);
		for (int i = 0; i < nPoints; i++)
			pGraphics->DrawLine((int)worldPoints[i].x, (int)worldPoints[i].y, (int)worldPoints[(i + 1) % nPoints].x, (int)worldPoints[(i + 1) % nPoints].y, RGB(0, 0, 255));
	}
	Vector2D GetNormal(int i) {
		return transformToWorld(points[(i + 1) % nPoints] - points[i]).InverseRotate().GetDirection();
	}
	Vector2D GetLowestPoint(Vector2D normal) {
		double lowest = 1e300;
		Vector2D ret;
		for (int i = 0; i < nPoints; i++) {
			double prod = normal * transformToWorld(points[i]);
			if (prod < lowest) {
				ret = transformToWorld(points[i]);
				lowest = prod;
			}
		}
		return ret;
	}
	void ChangePoint(int i, Vector2D p) {
		assert(0 <= i && i < nPoints);
		points[i] = p;
	}
	Vector2D GetPoint(int i) {
		return transformToWorld(points[i]);
	}
	bool PointInside(Vector2D p) {
		p = transformToWorldInverse(p);
		for (int i = 0; i < nPoints; i++)
			if ((p - points[i]) % (points[(i + 1) % nPoints] - points[i]) > eps) return false;
		return true;
	}
	double GetMass() {
		double ret = 0;
		for (int i = 0; i < nPoints; i++)
			ret += points[i] % points[(i + 1) % nPoints] / 2;
		ret *= density;
		return ret;
	}
	void UpdateMass() {
		mass = GetMass();
	}
	void UpdateCenterOfMass() {
		UpdateMass();
		Vector2D newCenter = Vector2D(0, 0, 0);
		for (int i = 0; i < nPoints; i++) {
			newCenter += (points[i] + points[(i + 1) % nPoints]) * (points[i] % points[(i + 1) % nPoints]) / 2;
		}
		newCenter /= (mass / density) * 3;
		newCenter.z = 0;
		for (int i = 0; i < nPoints; i++)
			points[i] -= newCenter;
	}
	void UpdateMomentOfInertia() {
		double ret = 0;
		vector<double> xList, yList;
		for (int i = 0; i < nPoints; i++) xList.push_back(points[i].x), yList.push_back(points[i].y);
		sort(xList.begin(), xList.end());
		sort(yList.begin(), yList.end());
		for (int i = 0; i < nPoints - 1; i++) {
			double x0 = xList[i], x1 = xList[i + 1];
			ret += (GetCutLengthX(x0) * x0 * x0 + 
				    GetCutLengthX((x0 + x1) / 2) * (x0 + x1) * (x0 + x1) + 
				    GetCutLengthX(x1) * x1 * x1) / 6.0 * (x1 - x0);
		}
		for (int i = 0; i < nPoints - 1; i++) {
			double y0 = yList[i], y1 = yList[i + 1];
			ret += (GetCutLengthY(y0) * y0 * y0 + 
				    GetCutLengthY((y0 + y1) / 2) * (y0 + y1) * (y0 + y1) + 
				    GetCutLengthY(y1) * y1 * y1) / 6.0 * (y1 - y0);
		}
		momentOfInertia = ret * density;
	}
	void Update() {
		UpdateCenterOfMass();
		UpdateMomentOfInertia();
		circleRadius = 0;
		for (int i = 0; i < nPoints; i++)
			circleRadius = max(circleRadius, points[i].GetLength());
	}
	void GetProjection(Vector2D normal, double &minI, double &maxI) {
		assert(nPoints);
		minI = maxI = transformToWorld(points[0]) * normal;
		for (int i = 1; i < nPoints; i++) {
			double prod = transformToWorld(points[i]) * normal;
			if (prod < minI) minI = prod;
			if (prod > maxI) maxI = prod;
		}
	}
	static Polygon GeneratePolygon(int nPoints, double r) {
		Polygon poly;
		for (int i = 0; i < nPoints; i++)
			poly.AddPoint(Vector2D(r * cos(2 * pi / nPoints * i), r * sin(2 * pi / nPoints * i), 1));
		poly.Update();
		poly.SetRotationAngle(-pi / nPoints);
		return poly;
	}
};

class Circle : public Shape {
private :
	double radius;
public :
	Circle() {
		radius = 1.;
		density = 1.;
		Update();
	}
	void SetRadius(double radius) {
		this->radius = radius;
	}
	double GetRadius() {
		return radius;
	}
	double GetMass() {
		return density * sqr(radius) * pi;
	}
	void UpdateMass() {
		mass = GetMass();
	}
	double GetMomentOfInertia() {
		return density * sqr(sqr(radius)) * pi / 2.; 
	}
	void UpdateMomentOfInertia() {
		momentOfInertia = GetMomentOfInertia();
	}
	void Update() {
		UpdateMass();
		UpdateMomentOfInertia();
		circleRadius = radius;
	}
};

#endif
