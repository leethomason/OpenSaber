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

void UIModeUtil::nextMode()
{
	static const int NUM = (int)UIMode::MEDITATION + 1;
	m_mode = (UIMode)((int(m_mode) + 1) % NUM);
}


int main(int, char**) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 800, 400, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
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
	Pixel_7_5_UI pixel75;

	SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);

	UIRenderData data;
	data.volume = 2;
	data.power = 3;
	data.mVolts = 3219;
	data.fontName = "Bespin";

	Timer2::Test();
	DotStarUI::Test();
	TestCStr();
	TestHex();
	TestUtil();

	SDL_Event e;
	int scale = 4;
	UIModeUtil mode;
	mode.set(UIMode::NORMAL);
	bool bladeOn = false;
	int count = 0;
	uint32_t lastUpdate = SDL_GetTicks();
	bool oledMode = false;

	const char* FONT_NAMES[8] = {
		"Bespin", "Vader", "Vader", "ObiAni", "Bespin", "JainaSw", "Maul", "MAUL"
	};
	uint8_t COLORS[8] = { 0, 255, 100, 200, 255, 0, 20, 120 };
	data.color.set(COLORS[0], COLORS[1], COLORS[2]);

	int palette = 0;

	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_8) {
				scale = e.key.keysym.sym - SDLK_0;
			}
			else if (e.key.keysym.sym == SDLK_SPACE) {
				if (mode.mode() == UIMode::NORMAL && !bladeOn) {
					bladeOn = true;
				}
				else {
					bladeOn = false;
					mode.nextMode();
				}
			}
			else if (e.key.keysym.sym == SDLK_p) {
				data.power = (data.power + 1) % 5;
				data.mVolts = 3000 + data.power * 111;
			}
			else if (e.key.keysym.sym == SDLK_v) {
				data.volume = (data.volume + 1) % 5;
			}
			else if (e.key.keysym.sym == SDLK_o) {
				oledMode = !oledMode;
			}
			else if (e.key.keysym.sym == SDLK_c) {
				palette = (palette + 1) % 8;
				data.color.set(COLORS[(palette * 3 + 0) % 8],
					COLORS[(palette * 2 + 1) % 8],
					COLORS[(palette * 5 + 2) % 8]);
				data.fontName = FONT_NAMES[palette];
				data.palette = palette;
			}
		}

		uint32_t t = SDL_GetTicks();
		if (t - lastUpdate > 100) {
			lastUpdate = t;
			uint8_t value = int(127.8 * (sin(count * 0.2) + 1.0));
			++count;
			sketcher.Push(value);
			sketcher.Draw(&display, 100, mode.mode(), bladeOn, &data);
			pixel75.Draw(t, mode.mode(), bladeOn, &data);
		}

		oled.Commit();

		const SDL_Rect src = { 0, 0, WIDTH, HEIGHT };
		SDL_Rect winRect;
		SDL_GetWindowSize(win, &winRect.w, &winRect.h);
		const int w = WIDTH * scale;
		const int h = HEIGHT * scale;
		SDL_Rect dst = { (winRect.w - w) / 2, (winRect.h - h) / 2, w, h };

		if (!oledMode) {
			oled.Clear();
			memcpy(oled.Buffer(), pixel75.Pixels(), 7);
			oled.Commit();
		}

		SDL_RenderClear(ren);
		SDL_UpdateTexture(texture, NULL, oled.Pixels(), WIDTH * 4);
		SDL_RenderCopy(ren, texture, &src, &dst);
		SDL_RenderPresent(ren);
	}
	SDL_Quit();
	return 0;
}
