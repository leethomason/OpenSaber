#include <stdio.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "display.h"
#include "oledsim.h"
#include "draw.h"

int line = 0;
uint32_t prevTime = 0;
uint32_t animTime = 0;

static const int WIDTH = 128;
static const int HEIGHT = 32;

void Draw(Display* d, uint32_t time)
{
	d->Fill(0);

	static const int DIAL_WIDTH = 28;

	d->DrawBitmap(0, 0, get_dial3);
	d->DrawBitmap(WIDTH - DIAL_WIDTH, 0, get_dial1, Display::FLIP_X);
	d->DrawStr("P", 22, 12, getGlypth_aurekBesh);
	d->DrawStr("V", 96, 12, getGlypth_aurekBesh);

	//d->DrawRectangle(WIDTH / 2 - 1, 2, 3, 10);

	static const char* lines[] = {
		"THERE IS NO IGNORANCE, THERE IS ATTENTION.",
		"THERE IS NO SELF, THERE IS THE FORCE"
	};

	if (prevTime) {
		animTime += time - prevTime;
	}
	prevTime = time;
	
	int dx = animTime / 100; // / 80;
	bool render = d->DrawStr(lines[line], WIDTH - DIAL_WIDTH - 1 - dx, 22, getGlypth_aurekBesh, 
							 DIAL_WIDTH, WIDTH - DIAL_WIDTH);
	if (!render && !line) {
		line = 1;
		animTime = 0;
	}
	
	/*
	const int FONT = 5;	// 0-based
	for (int i = 0; i <= FONT; ++i) {
		int x = i % 4;
		int y = i / 4;
		d->DrawRectangle(WIDTH / 2 - 25 + x*5, y*5, 4, 4);
	}

	const uint8_t color[3] = { 0, 255, 100 };
	for (int i = 0; i < 3; ++i) {
		d->DrawRectangle(WIDTH / 2 + 6, i * 3, color[i] * 20 / 255, 2);
	}
	*/
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

	SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
	assert(texture);

	OledSim oled(WIDTH, HEIGHT);
	Display display;
	display.Attach(WIDTH, HEIGHT, oled.Buffer());

	SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);

	SDL_Event e;
	int scale = 4;
	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT){
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_8) {
				scale = e.key.keysym.sym - SDLK_0;
			}
		}

		Draw(&display, SDL_GetTicks());
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
