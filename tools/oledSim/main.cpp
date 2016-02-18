#include <stdio.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "display.h"
#include "oledsim.h"
#include "draw.h"

int tOffset = 0;
int line = 0;

void Draw(Display* d, uint32_t time)
{
	d->Fill(0);

	d->DrawBitmap(0, 0, get_dial3);
	d->DrawBitmap(128-28, 0, get_dial1, Display::FLIP_X);

	static const char* lines[] = {
		"THERE IS NO IGNORANCE, THERE IS ATTENTION.",
		"THERE IS NO SELF, THERE IS THE FORCE"
	};

	int dx = (time - tOffset) / 50;
	bool render = d->DrawStr(lines[line], 128- 28 - 1 - dx, 22, getGlypth_aurekBesh, 28, 128 - 28);
	if (!render && !line) {
		line = 1;
		tOffset = time;
	}
	
	/*
	// Test pattern. dot-space-line
	uint8_t* buf = d->Buffer();
	*buf = 0xf1;
	*/
}


int main(int, char**){
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 800, 400, SDL_WINDOW_SHOWN);
	if (win == nullptr){
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert(ren);

	static const int WIDTH = 128;
	static const int HEIGHT = 32;

	SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
	assert(texture);

	OledSim oled(WIDTH, HEIGHT);
	Display display;
	display.Attach(WIDTH, HEIGHT, oled.Buffer());

	SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);

	SDL_Event e;
	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT){
			break;
		}

		Draw(&display, SDL_GetTicks());
		oled.Commit();

		const SDL_Rect src = { 0, 0, WIDTH, HEIGHT };
		SDL_Rect winRect;
		SDL_GetWindowSize(win, &winRect.w, &winRect.h);
		const int scale = 4;
		const int w = WIDTH * scale;
		const int h = HEIGHT * scale;
		SDL_Rect dst = { (winRect.w - w) / 2, (winRect.h - h) / 2, w, h };

		SDL_UpdateTexture(texture, NULL, oled.Pixels(), WIDTH * 4);
		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, texture, &src, &dst);
		SDL_RenderPresent(ren);
	}
	SDL_Quit();
	return 0;
}
