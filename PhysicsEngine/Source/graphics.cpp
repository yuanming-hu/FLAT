#include "graphics.h"

void ScreenObject::SetGraphics(Graphics *_graphics) {
	if (graphics != NULL) graphics->DeleteScreenObject(this);
	graphics = _graphics;
	if (graphics != NULL) graphics->AddScreenObject(this);
}
ScreenObject::~ScreenObject() {
	if (graphics != NULL) graphics->DeleteScreenObject(this);		
}

void Graphics::DrawLine(int x0, int y0, int x1, int y1, int color) {
	int dx = x1 - x0, dy = y1 - y0;
	if (dx == 0 && dy == 0) {
		DrawPixel(x0, y0, color);
		return;
	}
	if (abs(dx) > abs(dy)) {
		if (x0 > x1) swap(x0, x1), swap(y0, y1);
		for (int i = x0; i <= x1; i++) DrawPixel(i, y0 + (y1 - y0) * (i - x0) / (x1 - x0), color);
	}
	else {
		if (y0 > y1) swap(x0, x1), swap(y0, y1);
		for (int j = y0; j <= y1; j++) DrawPixel(x0 + (x1 - x0) * (j - y0) / (y1 - y0), j, color);
	}
}

void Graphics::DrawRect(int x, int y, int w, int h, int color) {
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + w > W) x = W - w - 1;
    if (y + h > H) y = H - h - 1;
    for (int i = x; i < x + w; i++)
        for (int j = y; j < y + h; j++)
            DrawPixel(i, j, color);
}

void Graphics::DrawCircle(int x, int y, int r, int color) {
	for (int i = -r; i <= r; i++) {
		int tmp = r * r - i * i;
		int j = (int)sqrt((double)tmp);
		DrawPixel(x + i, y + j, color);
		DrawPixel(x + i, y - j, color);
	}
	for (int j = -r; j <= r; j++) {
		int tmp = r * r - j * j;
		int i = (int)sqrt((double)tmp);
		DrawPixel(x - i, y + j, color);
		DrawPixel(x + i, y + j, color);
	}
}

