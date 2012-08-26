#include "game.h"

void Game::DrawingAndLineCrossTest() {
	double x = 0;
	while (1) {
		x += 0.05;
		graphics.Clean();
		Line l0(Vector2D(0, 0, 1), Vector2D(400, x, 1)), l1(Vector2D(400, 400, 1), Vector2D(0, 300 - x, 1));
		graphics.DrawLine((int)l0.GetA().x, (int)l0.GetA().y, (int)l0.GetB().x, (int)l0.GetB().y, RGB(0, 255, 0));
		graphics.DrawLine((int)l1.GetA().x, (int)l1.GetA().y, (int)l1.GetB().x, (int)l1.GetB().y, RGB(0, 255, 0));
		Vector2D p = l0 * l1;
		graphics.DrawCircle((int)p.x, (int)p.y, 10, RGB(255, 0, 0));
		graphics.ShowScreen();
	}
}

void Game::Initialize() {
	graphics.Initialize();
	physics.SetGraphics(&graphics);
}

/*
static void Game::GeneratePolygon() {

}
*/

void Game::Run() {
	Initialize();

	Polygon poly;

	poly.AddPoint(Vector2D(-300, -10, 1));
	poly.AddPoint(Vector2D(300, -10, 1));
	poly.AddPoint(Vector2D(300, 10, 1));
	poly.AddPoint(Vector2D(-300, 10, 1));
	poly.SetCenterPotision(Vector2D(400, 30, 1));
	poly.SetDensity(100);
//	poly.SetFixed(true);

	Shape *block = new Polygon(poly);
	
	physics.AddShape(block);
	physics.AddConstantConstraint(new DistanceConstraint(Vector2D(W / 2, 30, 1), 0., block, Vector2D(0, 0, 0)));

	/*
	poly.SetFixed(true);
	poly.ChangePoint(0, Vector2D(-10, -250, 1));
	poly.ChangePoint(1, Vector2D(10, -250, 1));
	poly.ChangePoint(2, Vector2D(10, 250, 1));
	poly.ChangePoint(3, Vector2D(-10, 250, 1));
	poly.SetCenterPotision(Vector2D(40, 270, 1));	
	physics.AddShape(new Polygon(poly));
	poly.SetCenterPotision(Vector2D(760, 270, 1));	
	physics.AddShape(new Polygon(poly));
	*/
	

	poly.SetDensity(1.);

	poly.SetFixed(false);
//	poly.SetRotationAngle(0.1);

	poly.ChangePoint(0, Vector2D(-90, -3, 1));
	poly.ChangePoint(1, Vector2D(90, -3, 1));
	poly.ChangePoint(2, Vector2D(90, 3, 1));
	poly.ChangePoint(3, Vector2D(-90, 3, 1));
	poly.Update();

	bool slow = false;
	
	int shapeId = 0;
	Vector2D P, Q; bool addingDisCon = false;
	for (int k = 0; ; k++) {
		graphics.Clean();
		input.Update();
		if (input.keyboard.NeedProcess(SDLK_s)) slow ^= 1;
		if (input.keyboard.NeedProcess(SDLK_3)) shapeId = 3;
		if (input.keyboard.NeedProcess(SDLK_4)) shapeId = 4;
		if (input.keyboard.NeedProcess(SDLK_5)) shapeId = 5;
		if (input.keyboard.NeedProcess(SDLK_6)) shapeId = 6;
		if (input.keyboard.NeedProcess(SDLK_7)) shapeId = 7;
		if (input.keyboard.NeedProcess(SDLK_8)) shapeId = 8;
		if (input.keyboard.NeedProcess(SDLK_9)) shapeId = 9;
		if (input.keyboard.NeedProcess(SDLK_0)) shapeId = 0;



		if (input.mouse.NeedProcess(1)) {
			
//			block = new Circle(Circle::GenerateCircle(40));
			switch(shapeId) {
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
				block = new Polygon(Polygon::GeneratePolygon(shapeId, 50));
				break;
			case 0:
				block = new Circle(Circle::GenerateCircle(50));
				break;
			default :
				break;
			}
			block->SetCenterPotision(Vector2D(input.mouse.GetX(), input.mouse.GetY(), 1));
			physics.AddShape(block);
//			physics.AddConstantConstraint(new DistanceConstraint(Vector2D(W / 2, H / 5 * 4, 1), 400., block, Vector2D(0, 0, 0), &graphics));
		}

		if (input.mouse.NeedProcess(3)) {
			if (addingDisCon) {
				Q = Vector2D(input.mouse.GetX(), input.mouse.GetY(), 1);
				Shape *shape; Vector2D r;
				physics.SelectShape(Q, shape, r);
				if (shape) {
					physics.AddConstantConstraint(new DistanceConstraint(P, (P - Q).GetLength(), shape, r, &graphics));			
				}
			} else {
				P = Vector2D(input.mouse.GetX(), input.mouse.GetY(), 1);
			}
			addingDisCon = !addingDisCon;
		}
		if (addingDisCon) {
			graphics.DrawLine((int)P.x, (int)P.y, input.mouse.GetX(), input.mouse.GetY(), RGB(255, 0, 0));
		}
		if (!slow || (slow && input.keyboard.NeedProcess(SDLK_DOWN))) {
			physics.Proceed(timeInterval);
		}
		graphics.Redraw();
//		SDL_Delay(10);
	}
}
