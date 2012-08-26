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
	physics.SetpGraphics(&graphics);
}

/*
static void Game::GeneratePolygon() {

}
*/

void Game::Run() {
	Initialize();

	Polygon poly;

	/*
	poly.AddPoint(Vector2D(-350, -5, 1));
	poly.AddPoint(Vector2D(350, -5, 1));
	poly.AddPoint(Vector2D(350, 5, 1));
	poly.AddPoint(Vector2D(-350, 5, 1));
	poly.SetCenterPotision(Vector2D(400, 20, 1));	
	poly.SetFixed(true);

	physics.AddPolygons(new Polygon(poly));
	*/
	poly.AddPoint(Vector2D(-250, -10, 1));
	poly.AddPoint(Vector2D(250, -10, 1));
	poly.AddPoint(Vector2D(250, 10, 1));
	poly.AddPoint(Vector2D(-250, 10, 1));
	poly.SetCenterPotision(Vector2D(400, 30, 1));
	poly.SetDensity(100);
	poly.SetFixed(true);

	Polygon *block = new Polygon(poly);
	
	physics.AddPolygon(block);
	physics.AddConstantConstraint(new DistanceConstraint(Vector2D(W / 2, 30, 1), 0., block, Vector2D(0, 0, 0)));
	/*
	poly.ChangePoint(0, Vector2D(-10, -250, 1));
	poly.ChangePoint(1, Vector2D(10, -250, 1));
	poly.ChangePoint(2, Vector2D(10, 250, 1));
	poly.ChangePoint(3, Vector2D(-10, 250, 1));
	poly.SetCenterPotision(Vector2D(40, 270, 1));	
	physics.AddPolygons(new Polygon(poly));

	poly.SetCenterPotision(Vector2D(760, 270, 1));	
	physics.AddPolygons(new Polygon(poly));
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

	for (int k = 0; ; k++) {
		input.Update();
		if (input.keyboard.NeedProcess(SDLK_s)) slow ^= 1;
		if (input.keyboard.NeedProcess(SDLK_3)) poly = Polygon::GeneratePolygon(3, 40);
		if (input.keyboard.NeedProcess(SDLK_4)) poly = Polygon::GeneratePolygon(4, 40);
		if (input.keyboard.NeedProcess(SDLK_5)) poly = Polygon::GeneratePolygon(5, 40);
		if (input.keyboard.NeedProcess(SDLK_6)) poly = Polygon::GeneratePolygon(6, 40);
		if (input.keyboard.NeedProcess(SDLK_7)) poly = Polygon::GeneratePolygon(7, 40);
		if (input.keyboard.NeedProcess(SDLK_8)) poly = Polygon::GeneratePolygon(8, 40);
		if (input.keyboard.NeedProcess(SDLK_9)) poly = Polygon::GeneratePolygon(9, 40);
		if (input.keyboard.NeedProcess(SDLK_0)) poly = Polygon::GeneratePolygon(10, 40);

		if (input.mouse.NeedProcess(1)) {
			poly.SetCenterPotision(Vector2D(input.mouse.GetX(), input.mouse.GetY(), 1));
			block = new Polygon(poly);
			physics.AddPolygon(block);
//			physics.AddConstantConstraint(new DistanceConstraint(Vector2D(W / 2, H / 5 * 4, 1), 400., block, Vector2D(0, 0, 0), &graphics));
		}
		if (!slow || (slow && input.keyboard.NeedProcess(SDLK_DOWN))) {
			graphics.Clean();
			physics.Proceed(timeInterval);
			graphics.Redraw();
		}
//		SDL_Delay(10);
	}
}
