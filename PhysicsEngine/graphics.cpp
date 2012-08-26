#include "graphics.h"

void ScreenObject::SetpGraphics(Graphics *_pGraphics) {
	if (pGraphics != NULL) pGraphics->DeletepScreenObject(this);
	pGraphics = _pGraphics;
	if (pGraphics != NULL) pGraphics->AddpScreenObject(this);
}
ScreenObject::~ScreenObject() {
	if (pGraphics != NULL) pGraphics->DeletepScreenObject(this);		
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
	for (int i = x - r; i < x + r; i++)
		for (int j = y - r; j < y + r; j++)
			if ((i - x) * (i - x) + (j - y) * (j - y) <= r * r) DrawPixel(i, j, color);
}

