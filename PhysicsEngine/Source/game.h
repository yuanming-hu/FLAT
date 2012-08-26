#ifndef GAME_H
#define GAME_H

#include "graphics.h"
#include "physics.h"
#include "input.h"
#include "polygon.h"

class Game {
private:
	Graphics graphics;
	Input input;
	Physics physics;
public:
	void DrawingAndLineCrossTest();
	void Initialize();
	void Run();
};

#endif