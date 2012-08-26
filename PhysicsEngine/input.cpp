#include "input.h"

void Input::Update() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_MOUSEMOTION:
				mouse.x = event.motion.x;
				mouse.y = H - event.motion.y;
				break;

			case SDL_MOUSEBUTTONDOWN:
				mouse.KeyDown(event.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				mouse.KeyUp(event.button.button);
				break;

			case SDL_KEYDOWN:
				keyboard.KeyDown(event.key.keysym.sym);
				break;
			case SDL_KEYUP:
				keyboard.KeyUp(event.key.keysym.sym);
				break;

			case SDL_QUIT:
				exit(0);
		}
	}
}
