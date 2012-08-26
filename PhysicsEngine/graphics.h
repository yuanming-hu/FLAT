#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "constants.h"
#include <SDL.h>
#include <algorithm>
using namespace std;

class Graphics;

class ScreenObject {
protected:
	Graphics *pGraphics;
public:
	ScreenObject() {pGraphics = NULL;}
	void SetpGraphics(Graphics *_pGraphics);
	virtual void Redraw() = 0;
	~ScreenObject();
};

class Graphics {
private:
	SDL_Surface *screen;
    int pixels[W][H];
	static const int constMaxNpScreenObjects = 512;
	ScreenObject *pScreenObjects[constMaxNpScreenObjects];
	int npScreenObjects;
public:
	int Initialize() {
		npScreenObjects = 0;
		if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
			fprintf(stderr, "Failed To Initialize SDL: %s\n", SDL_GetError());
			return 1; 
		}
		screen = SDL_SetVideoMode(W, H, 32, SDL_SWSURFACE);
/*
		if (screen == NULL) {
            fprintf(stderr, "无法设置%d x %d的视频模式：%s\n", W, H, SDL_GetError());
			return 1;
        }
        printf("Screen Initialization----OK!");
        printf("%d\n", screen->pitch);
*/
        return 0;	
	}
	void ShowScreen() {
		SDL_UpdateRect(screen, 0, 0, W, H);
    }

	void Clean(int darkness = 0) {
		memset(screen->pixels, darkness, W * H * 4);
		return;
/*
		for (int y = 0; y < H; y++) 
			for (int x = 0; x < W; x++) {
				Uint32 color = *((Uint32 *)screen->pixels + (H - y - 1) * screen->pitch / 4 + x);
				Uint32 R = (color >> 16) & 255;
				Uint32 G = (color >> 8) & 255;
				Uint32 B = (color) & 255;
//				DrawPixel(x, y, RGB(R >> 1, G >> 1, B >> 1));
//				*((Uint32 *)screen->pixels + (H - y - 1) * screen->pitch / 4 + x) = RGB(R / 2, G / 2, B / 2);
			}
*/
	}
	void DrawPixel(int x, int y, int color) {
    	if (x < 0 || x >= W || y < 0 || y >= H) return;
		y = H - y - 1;
		
		int r = color >> 16;
		int g = (color >> 8) & 0xff;
		int b = color & 0xff;

		* (int*) ( (char*)screen->pixels
				+ y*screen->pitch
				+ x*screen->format->BytesPerPixel )
			= (r << screen->format->Rshift)
			+ (g << screen->format->Gshift)
			+ (b << screen->format->Bshift);
    }
	void DrawLine(int x0, int y0, int x1, int y1, int color);
    void DrawRect(int x, int y, int w, int h, int color);
	void DrawCircle(int x, int y, int r, int color);
	void Redraw() {
//		Clean();
		for (int i = 0; i < npScreenObjects; i++)
			pScreenObjects[i]->Redraw();
		ShowScreen();
	}
	void AddpScreenObject(ScreenObject *pScreenObject) {
		pScreenObjects[npScreenObjects++] = pScreenObject;
	}
	void DeletepScreenObject(ScreenObject *pScreenObject) {
//		for (int i = 0; i < npScreenObjects; i++) if (pScreenObjects[i] == pScreenObject) {
//			for (int j = i; j < npScreenObjects - 1; j++)
//				pScreenObjects[j] = pScreenObjects[j + 1];
//			npScreenObjects--;
//		}
	}
};

#endif
