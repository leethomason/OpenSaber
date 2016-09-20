#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "isin.h"

static const int WIDTH = 256;
static const int HEIGHT = 192;

void calcColor(uint32_t t, const uint8_t* base, uint8_t* out)
{
	static const int32_t RATIO = 200;
	static const int32_t RATIO_M1 = 256 - RATIO;
	static const int32_t DIV = 256;

	uint32_t tc[3] = { t / 29, t / 37, t / 31 };

	for (int i = 0; i < 3; ++i) {
		int32_t s = isin(tc[i]);
		int32_t scaledColor = int32_t(base[i]) * RATIO + s * RATIO_M1;
		if (scaledColor < 0) scaledColor = 0;
		assert(scaledColor <= 65535);
		out[i] = uint8_t(scaledColor / DIV);
	}
}

struct RGB { 
	uint8_t r, g, b;
};

static const RGB baseColors[6] = {
	{ 0, 255, 0},
	{ 0, 0, 255 },
	{ 0, 255, 255 },
	{ 255, 0, 0},
	{ 200, 0, 200 },
	{ 0, 128, 255}
};

int main(int, char**) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert(ren);

	SDL_Event e;
	int currentBase = 0;
	uint32_t baseTime = 0;

	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_SPACE) {
				currentBase = (currentBase + 1) % 6;
				baseTime = SDL_GetTicks();
			}
		}

		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		
		static const int GAP = 8;
		static const int D = WIDTH / 5;

		SDL_Rect rects[] = {
			{ WIDTH / 4, HEIGHT / 4, WIDTH / 2, HEIGHT / 2},
			{ WIDTH / 4, HEIGHT / 4 - HEIGHT / 16 - GAP, WIDTH / 2, HEIGHT / 16 },
			{ WIDTH / 4, HEIGHT * 3 / 4 + GAP, WIDTH / 2, HEIGHT / 16 },
			{ WIDTH / 4 - GAP - D, HEIGHT / 2 - D / 2, D, D},
			{ WIDTH * 3 / 4 + GAP, HEIGHT / 2 - D / 2, D, D }
		};
		uint8_t c[3];
		uint8_t base[3] = { baseColors[currentBase].r, baseColors[currentBase].g, baseColors[currentBase].b };
		calcColor(SDL_GetTicks() - baseTime, base, c);

		SDL_SetRenderDrawColor(ren, c[0], c[1], c[2], 255);
		SDL_RenderFillRects(ren, rects, 5);
		SDL_RenderPresent(ren);
	}
	SDL_Quit();
	return 0;
}
