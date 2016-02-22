#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "renderer.h"
#include "oledsim.h"
#include "assets.h"
#include "sketcher.h"

Sketcher sketcher;
static const int WIDTH = 128;
static const int HEIGHT = 32;

void Draw(Renderer* d, uint32_t time, bool restMode)
{
	sketcher.Draw(d, time, restMode);
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

	SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
	assert(texture);

	OledSim oled(WIDTH, HEIGHT);
	Renderer display;
	display.Attach(WIDTH, HEIGHT, oled.Buffer());

	SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);

	SDL_Event e;
	int scale = 4;
	bool restMode = true;
	sketcher.volume = 2;
	sketcher.power = 3;
	sketcher.color[0] = 0;
	sketcher.color[1] = 255;
	sketcher.color[2] = 100;
	int count = 0;
	uint32_t lastUpdate = SDL_GetTicks();

	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT){
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_8) {
				scale = e.key.keysym.sym - SDLK_0;
			}
			else if (e.key.keysym.sym == SDLK_SPACE) {
				restMode = !restMode;
			}
			else if (e.key.keysym.sym == SDLK_p) {
				sketcher.power = (sketcher.power + 1) % 5;
			}
			else if (e.key.keysym.sym == SDLK_v) {
				sketcher.volume = (sketcher.volume + 1) % 5;
			}
			else if (e.key.keysym.sym == SDLK_c) {
				uint8_t c0 = sketcher.color[0];
				uint8_t c1 = sketcher.color[1];
				uint8_t c2 = sketcher.color[2];
				sketcher.color[0] = c1;
				sketcher.color[1] = c2;
				sketcher.color[2] = c0;
			}
		}

		uint32_t t = SDL_GetTicks();
		if (t - lastUpdate > 100) {
			lastUpdate = t;
			uint8_t value = int(127.8 * (sin(count * 0.2) + 1.0));
			++count;
			sketcher.Push(value);
		}

		Draw(&display, SDL_GetTicks(), restMode);
		oled.Commit();

		const SDL_Rect src = { 0, 0, WIDTH, HEIGHT };
		SDL_Rect winRect;
		SDL_GetWindowSize(win, &winRect.w, &winRect.h);
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
