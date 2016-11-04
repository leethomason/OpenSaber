#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "../../src/saber/sketcher.h"

static const int WIDTH = 256;
static const int HEIGHT = 192;

RGB baseColors[6];

int main(int, char**) {
	baseColors[0].set(0x00ff00);
	baseColors[1].set(0x0000ff);
	baseColors[2].set(0x00ffff);
	baseColors[3].set(0xff0000);
	baseColors[4].set(0xa000a0);
	baseColors[5].set(0x0080ff);

	TestCrystalColor();

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
		calcCrystalColor(SDL_GetTicks() - baseTime, 80, base, c);

		SDL_SetRenderDrawColor(ren, c[0], c[1], c[2], 255);
		SDL_RenderFillRects(ren, rects, 5);
		SDL_RenderPresent(ren);
	}
	SDL_Quit();
	return 0;
}
