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
	Polygon *polygonA, *polygonB;
	Vector2D p, n;
	double elasticConstant;
	double depth;
public:
	Contact(Polygon *a, Polygon *b, Vector2D _p, Vector2D _n, double _e, double _depth) : polygonA(a), polygonB(b), p(_p), n(_n), elasticConstant(_e), depth(_depth) {}
	void Process() {
		Vector2D v10 = polygonB->PointVelocity(p) - polygonA->PointVelocity(p);
		Vector2D r0 = p - polygonA->centerPosition, r1 = p - polygonB->centerPosition;
		double v0 = -n * v10;

		Vector2D tao = n.Rotate();
		double mu = 2;

		double J = (0.4 / timeInterval * max(0., depth - 0.1) + (1 + elasticConstant) * v0) / 
			(1 / polygonA->mass + 1 / polygonB->mass + sqr(r0 % n) / polygonA->momentOfInertia + sqr(r1 % n) / polygonB->momentOfInertia);
		if (J < 0) return;

		double j = -(v10 * tao) / (1 / polygonA->mass + 1 / polygonB->mass + sqr(r0 % tao) / polygonA->momentOfInertia + sqr(r1 % tao) / polygonB->momentOfInertia);
		
		j = max(min(j, mu * J), -mu * J);
		Vector2D impluse = J * n + j * tao;
		polygonA->AddImpluse(p, -impluse, true);
		polygonB->AddImpluse(p, impluse, true);
	}
	Constraint *Copy() {
		return new Contact(*this);
	}
};

class DistanceConstraint : public Constraint {
private:
	Vector2D p;
	double L;
	Polygon *poly;
	Vector2D r;
	Graphics *graphics;
public:
	DistanceConstraint(Vector2D p, double L, Polygon *poly, Vector2D r, Graphics *graphics = 0) : p(p), L(L), poly(poly), r(r), graphics(graphics) {}
	void Process() {
		Vector2D q = poly->centerPosition + poly->transformToWorld(r);
		if (graphics)
			graphics->DrawLine((int)p.x, (int)p.y, (int)q.x, (int)q.y, RGB(128, 0, 127));
		Vector2D n = p - q;
		double l = n.GetLength();
		if (l < eps) return;
		n.Normalize();
		double v = n * poly->PointVelocity(q);
		double J = ((l - L) * 0.3 / timeInterval - v) / (1.0 / poly->mass + sqr(r % n) / poly->momentOfInertia);
		poly->AddImpluse(q, n * J);
	}
	Constraint *Copy() {
		return new DistanceConstraint(*this);
	}
};

class Physics {
private:
	static const int constMaxnPolygons = 512;
	int nPolygons;
	Polygon *polygons[constMaxnPolygons];
	Graphics *pGraphics;
	vector<Constraint *> constraints;
	vector<Constraint *> constantConstraints;
public:
	Physics() {
		nPolygons = 0;
	}
	void SetpGraphics(Graphics *_pGraphics) {
		pGraphics = _pGraphics;
		for (int i = 0; i < nPolygons; i++)
			polygons[i]->SetpGraphics(pGraphics);
	}
	void AddPolygon(Polygon *poly) {
		poly->Update();
		polygons[nPolygons] = poly;
		polygons[nPolygons]->SetpGraphics(pGraphics);
		nPolygons++;
	}
	void AddConstantConstraint(Constraint *constraint) {
		constantConstraints.push_back(constraint);
	}
	void DeleteLastPolygon() {
		nPolygons--;
	}
	void Proceed(double T) {
//		CleanRubbish();
		for (int i = 0; i < nPolygons; i++)
			polygons[i]->AddImpluse(polygons[i]->centerPosition, Vector2D(0, -9.8, 0) * polygons[i]->mass * T, true);
		SolveSituation();
		for (int i = 0; i < nPolygons; i++)
			polygons[i]->Proceed(T);
	}
	void SolveSituation() {
		for (int i = 0; i < (int)constantConstraints.size(); i++)
			constraints.push_back(constantConstraints[i]->Copy());
		for (int i = 0; i < nPolygons; i++)
			for (int j = i + 1; j < nPolygons; j++)
				TestCollision(polygons[i], polygons[j]);
		for (int K = 0; K < 30; K++)
			for (int i = 0; i < (int)constraints.size(); i++)
				constraints[i]->Process();
		for (; !constraints.empty(); ) {
			delete constraints.back();
			constraints.pop_back();
		}
	}
	void CleanRubbish() {
		for (int i = 0; i < nPolygons; i++) {
			if ((polygons[i]->centerPosition - Vector2D(0, 0, 1)).GetLength() > 10000) {
				for (int j = i; j < nPolygons - 1; j++) polygons[j] = polygons[j + 1];
				nPolygons--;
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
};

#endif
