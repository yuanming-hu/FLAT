#ifndef PHYSICS_H
#define PHYSICS_H

#include "polygon.h"

class Constraint {
public:
//	Constraint() {}
	virtual void Process() = 0;
	virtual Constraint *Copy() = 0;
};

class Contact : public Constraint{
	Shape *shapeA, *shapeB;
	Vector2D p, n;
	double elasticConstant;
	double depth;
public:
	Contact(Shape *a, Shape *b, Vector2D _p, Vector2D _n, double _e, double _depth) : shapeA(a), shapeB(b), p(_p), n(_n), elasticConstant(_e), depth(_depth) {}
	void Process() {
		Vector2D v10 = shapeB->PointVelocity(p) - shapeA->PointVelocity(p);
		Vector2D r0 = p - shapeA->centerPosition, r1 = p - shapeB->centerPosition;
		double v0 = -n * v10;

		Vector2D tao = n.Rotate();
		double mu = 2;

		double J = (0.4 / timeInterval * max(0., depth - 0.1) + (1 + elasticConstant) * v0) / 
			(1 / shapeA->mass + 1 / shapeB->mass + sqr(r0 % n) / shapeA->momentOfInertia + sqr(r1 % n) / shapeB->momentOfInertia);
		if (J < 0) return;

		double j = -(v10 * tao) / (1 / shapeA->mass + 1 / shapeB->mass + sqr(r0 % tao) / shapeA->momentOfInertia + sqr(r1 % tao) / shapeB->momentOfInertia);
		
		j = max(min(j, mu * J), -mu * J);
		Vector2D impluse = J * n + j * tao;
		shapeA->AddImpluse(p, -impluse, true);
		shapeB->AddImpluse(p, impluse, true);
	}
	Constraint *Copy() {
		return new Contact(*this);
	}
};

class DistanceConstraint : public Constraint {
private:
	Vector2D p;
	double L;
	Shape *shape;
	Vector2D r;
	Graphics *graphics;
public:
	DistanceConstraint(Vector2D p, double L, Shape *shape, Vector2D r, Graphics *graphics = 0) : p(p), L(L), shape(shape), r(r), graphics(graphics) {}
	void Process() {
		Vector2D q = shape->centerPosition + shape->transformToWorld(r);
		if (graphics)
			graphics->DrawLine((int)p.x, (int)p.y, (int)q.x, (int)q.y, RGB(128, 0, 127));
		Vector2D n = p - q;
		double l = n.GetLength();
		if (l < eps) return;
		n.Normalize();
		double v = n * shape->PointVelocity(q);
		double J = ((l - L) * 0.3 / timeInterval - v) / (1.0 / shape->mass + sqr(r % n) / shape->momentOfInertia);
		shape->AddImpluse(q, n * J);
	}
	Constraint *Copy() {
		return new DistanceConstraint(*this);
	}
};

class Physics {
private:
	static const int constMaxnShapes = 512;
	int nShapes;
	Shape *shapes[constMaxnShapes];
	Graphics *graphics;
	vector<Constraint *> constraints;
	vector<Constraint *> constantConstraints;
public:
	Physics() {
		nShapes = 0;
	}
	void SetGraphics(Graphics *_graphics) {
		graphics = _graphics;
		for (int i = 0; i < nShapes; i++)
			shapes[i]->SetGraphics(graphics);
	}
	void AddShape(Shape *shape) {
		shape->Update();
		shapes[nShapes] = shape;
		shapes[nShapes]->SetGraphics(graphics);
		nShapes++;
	}
	void AddConstantConstraint(Constraint *constraint) {
		constantConstraints.push_back(constraint);
	}
	void DeleteLastShape() {
		nShapes--;
	}
	void DeleteShape(Shape *shape) {
		for (int i = 0; i < nShapes; i++)
			if (shapes[i] == shape) {
				delete shapes[i];
				for (int j = i; j < nShapes - 1; j++)
					shapes[j] = shapes[j + 1];
				nShapes--;
			}
	}
	void Proceed(double T) {
		CleanRubbish();
		for (int i = 0; i < nShapes; i++)
			shapes[i]->AddImpluse(shapes[i]->centerPosition, Vector2D(0, -9.8, 0) * shapes[i]->mass * T, true);
		SolveSituation();
		for (int i = 0; i < nShapes; i++)
			shapes[i]->Proceed(T);
	}
	void SolveSituation() {
		for (int i = 0; i < (int)constantConstraints.size(); i++)
			constraints.push_back(constantConstraints[i]->Copy());
		for (int i = 0; i < nShapes; i++)
			for (int j = i + 1; j < nShapes; j++)
				TestCollision(shapes[i], shapes[j]);
		for (int K = 0; K < 30; K++)
			for (int i = 0; i < (int)constraints.size(); i++)
				constraints[i]->Process();
		for (; !constraints.empty(); ) {
			delete constraints.back();
			constraints.pop_back();
		}
	}
	void CleanRubbish() {
		for (int i = 0; i < nShapes; i++) {
			if ((shapes[i]->centerPosition - Vector2D(0, 0, 1)).GetLength() > 5000) {
				for (int j = i; j < nShapes - 1; j++) shapes[j] = shapes[j + 1];
				nShapes--;
				CleanRubbish();
			}
		}
	}
	bool TestCollision(Polygon *a, Polygon *b) {
		if ((a->centerPosition - b->centerPosition).GetLength() > a->circleRadius + b->circleRadius) return false;
		Vector2D n;
		Line edge;
		double minDepth = 1e300;
		int flg = 0;
		for (int i = 0; i < a->nPoints; i++) {
			double min0, max0, min1, max1;
			Vector2D normal = a->GetNormal(i);
			a->GetProjection(normal, min0, max0);
			b->GetProjection(normal, min1, max1);
			if (max0 < min1 || max1 < min0) return false;
			if (min1 < max0 && max0 - min1 < minDepth) {
				flg = 0;
				minDepth = max0 - min1;
				n = normal;
				edge = Line(a->GetPoint(i), a->GetPoint((i + 1) % a->nPoints));
			}
		}
		for (int i = 0; i < b->nPoints; i++) {
			double min0, max0, min1, max1;
			Vector2D normal = b->GetNormal(i);
			b->GetProjection(normal, min0, max0);
			a->GetProjection(normal, min1, max1);
			if (max0 < min1 || max1 < min0) return false;
			if (min1 < max0 && max0 - min1 < minDepth) {
				flg = 1;
				minDepth = max0 - min1;
				n = normal;
				edge = Line(b->GetPoint(i), b->GetPoint((i + 1) % b->nPoints));
			}
		}
		if (flg == 0) {
			for (int i = 0; i < b->nPoints; i++) {
				Vector2D p = b->GetPoint(i);
				double depth = -(p - edge.GetA()) * n;
				if (depth > -eps)
					constraints.push_back(new Contact(a, b, p, n, 0.0, depth));
			}
		} else {
			for (int i = 0; i < a->nPoints; i++) {
				Vector2D p = a->GetPoint(i);
				double depth = -(p - edge.GetA()) * n;
				if (depth > -eps)
					constraints.push_back(new Contact(b, a, p, n, 0.0, depth));
			}
		}
		return true;
	}
	bool TestCollision(Circle *a, Circle *b) {
		Vector2D n = (b->centerPosition - a->centerPosition);
		double depth = a->radius + b->radius - n.GetLength();
		if (depth < 0) return false;
		n.Normalize();
		constraints.push_back(new Contact(a, b, a->centerPosition + n * a->radius, n, 0.0, depth));
		return true;
	}
	bool TestCollision(Circle *a, Polygon *b) {
		if ((a->centerPosition - b->centerPosition).GetLength() > a->circleRadius + b->circleRadius) return false;
		for (int i = 0; i < b->nPoints; i++) {
			Vector2D p = b->transformToWorld(b->points[i]);
			if (a->PointInside(p)) {
				double depth = a->radius - (a->centerPosition - p).GetLength();
				constraints.push_back(new Contact(a, b, p, (p - a->centerPosition).GetDirection(), 0.0, depth));
				return true;
			}
		}
		Vector2D n, p;
		Line edge;
		double minDepth = 1e300;
		for (int i = 0; i < b->nPoints; i++) {
			double min0, max0, min1, max1;
			Vector2D normal = b->GetNormal(i);
			b->GetProjection(normal, min0, max0);
			a->GetProjection(normal, min1, max1);
			if (max0 < min1 || max1 < min0) return false;
			if (min1 < max0 && max0 - min1 < minDepth) {
				minDepth = max0 - min1;
				n = normal;
				edge = Line(b->GetPoint(i), b->GetPoint((i + 1) % b->nPoints));
				p = edge.GetA() + (a->centerPosition - edge.GetA()) * edge.GetV().GetDirection() * edge.GetV().GetDirection();
			}
		}
		constraints.push_back(new Contact(b, a, p, n, 0.0, minDepth));
		return false;
	}
	bool TestCollision(Shape *a, Shape *b) {
		int typeA = a->GetType(), typeB = b->GetType();
		if (typeA == Polygon::ShapeType && typeB == Polygon::ShapeType)
			return TestCollision((Polygon *)a, (Polygon *)b);
		if (typeA == Circle::ShapeType && typeB == Circle::ShapeType)
			return TestCollision((Circle *)a, (Circle *)b);
		if (typeA == Circle::ShapeType && typeB == Polygon::ShapeType)
			return TestCollision((Circle *)a, (Polygon *)b);
		if (typeA == Polygon::ShapeType && typeB == Circle::ShapeType)
			return TestCollision((Circle *)b, (Polygon *)a);
		return false;
	}
	void SelectShape(Vector2D p, Shape *&shape, Vector2D &r) {
		for (int i = 0; i < nShapes; i++)
			if (shapes[i]->PointInside(p)) {
				shape = shapes[i];
				r = shapes[i]->transformToWorldInverse(p - shapes[i]->centerPosition);
				return;
			}
		shape = NULL;
	}
};

#endif
