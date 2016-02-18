#include <stdio.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "display.h"
#include "draw.h"

int tOffset = 0;
int line = 0;

void Draw(Display* d, uint32_t time)
{
	d->Fill(0);

	int texW = 0, texH = 0, texR = 0, advance = 0;
	const uint8_t* tex = get_hw(&texW, &texH);
	/*
	d->DrawBitmap(6, 0, tex, texW, texH);
	//d->DrawBitmap(20, 0, tex, texW, texH);
	//d->DrawBitmap(120, 0, tex, texW, texH);
	
	tex = get_tt(&texW, &texH);
	d->DrawBitmap(40, 5, tex, texW, texH);
	//d->DrawBitmap(60, 4, tex, texW, texH);
	d->DrawBitmap(60, 7, tex, texW, texH);
	d->DrawBitmap(80, 8, tex, texW, texH);
	d->DrawBitmap(100, 20, tex, texW, texH);
	*/
	/*
	tex = getGlypth_consolas('A', &advance, &texW, &texR);
	texH = texR * 8;
	d->DrawBitmap(0, 0, tex, texW, texH, false);

	int x = advance;
	tex = getGlypth_aurekBesh('B', &advance, &texW, &texR);
	texH = texR * 8;
	d->DrawBitmap(x, 0, tex, texW, texH, false);
	*/
	static const char* lines[] = {
		"THERE IS NO IGNORANCE, THERE IS ATTENTION.",
		"THERE IS NO SELF, THERE IS THE FORCE"
	};

	int dx = (time - tOffset) / 50;
	bool render = d->DrawStr(lines[line], 107 - dx, 22, getGlypth_aurekBesh, 10, 108);
	if (!render && !line) {
		line = 1;
		tOffset = time;
	}
	
	//d->DrawStr("THERE IS NO IGNORANCE", 10, 22, getGlypth_consolas, 10, 108);

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

	Display display(WIDTH, HEIGHT);
	SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);

	SDL_Event e;
	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT){
			break;
		}

		Draw(&display, SDL_GetTicks());
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
