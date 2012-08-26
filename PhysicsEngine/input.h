#ifndef INPUT_H
#define INPUT_H

#include "constants.h"
#include <SDL.h>

class Mouse {
	friend class Input;
private:
	int x, y;
	int clickCount[256];
	bool pressed[256];
	void KeyDown(int key) {
//		printf("%d\n", key);
		pressed[key] = true;
		clickCount[key]++;
	}
	void KeyUp(int key) {
		pressed[key] = false;
	}
public:
	Mouse() {
		memset(clickCount, 0, sizeof(clickCount));
		memset(pressed, 0, sizeof(pressed));
	}
	int GetX() {return x;}
	int GetY() {return y;}
	bool IsPressed(int key) {
		return pressed[key];
	}
	bool NeedProcess(int key) {
		if (clickCount[key]) {
			clickCount[key]--;
			return true;
		}
		return false;
	}
};

struct Keyboard {
	friend class Input;
private:
	bool pressed[256];
	int clickCount[256];
	void KeyDown(int key) {
		pressed[key] = true;
		clickCount[key]++;
	}
	void KeyUp(int key) {
		pressed[key] = false;
	}
public:
	Keyboard() {
		memset(pressed, 0, sizeof(pressed));
		memset(clickCount, 0, sizeof(clickCount));
	}
	bool NeedProcess(int key) {
		if (clickCount[key]) {
			clickCount[key]--;
			return true;
		}
		return false;
	}
};

class Input {
public:
	Mouse mouse;
	Keyboard keyboard;
	void Update();
};

#endif
