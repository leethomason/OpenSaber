#include <stdio.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "display.h"
#include "draw.h"

void Draw(Display* d)
{
	int texW = 0, texH = 0;
	const uint8_t* tex = get_hw(&texW, &texH);
	d->DrawBitmap(-6, 0, tex, texW, texH);
	d->DrawBitmap(20, 0, tex, texW, texH);
	d->DrawBitmap(120, 0, tex, texW, texH);
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

	Display display(WIDTH, HEIGHT);
	SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);

	SDL_Event e;
	while (SDL_WaitEvent(&e)){
		if (e.type == SDL_QUIT){
			break;
		}

		Draw(&display);
		display.Commit();

		const SDL_Rect src = { 0, 0, WIDTH, HEIGHT };
		SDL_Rect winRect;
		SDL_GetWindowSize(win, &winRect.w, &winRect.h);
		const int scale = 4;
		const int w = WIDTH * scale;
		const int h = HEIGHT * scale;
		SDL_Rect dst = { (winRect.w - w) / 2, (winRect.h - h) / 2, w, h };

		SDL_UpdateTexture(texture, NULL, display.Pixels(), WIDTH * 4);
		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, texture, &src, &dst);
		SDL_RenderPresent(ren);
	}
	SDL_Quit();
	return 0;
}
