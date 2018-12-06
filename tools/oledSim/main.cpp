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
#include "../saber/unittest.h"
#include "../saber/rgb.h"

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
    osbr::RGB dotstar4[4];
    osbr::RGB dotstar6[6];
    DotStarUI dotstarUI;

	SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);

	UIRenderData data;
	data.volume = 2;
	data.mVolts = 3219;
	data.fontName = "Bespin";

    runUnitTests();
    
	SDL_Event e;
	int scale = 4;
	UIModeUtil mode;
	mode.set(UIMode::NORMAL);
	bool bladeOn = false;
	int count = 0;
	uint32_t lastUpdate = SDL_GetTicks();

    enum {
        RENDER_OLED,
        RENDER_MATRIX,
        RENDER_DOTSTAR_4,
        RENDER_DOTSTAR_6,
        NUM_RENDER_MODE
    };
	int renderMode = RENDER_OLED;

	const char* FONT_NAMES[8] = {
		"Bespin", "Tatoine", "Jaina", "Vader", "ObiAni", "Bespin", "JainaSw", "Sentinel"
	};
    osbr::RGB colors[8] = {
        osbr::RGB(0, 255, 0),
        osbr::RGB(0, 0, 255),
        osbr::RGB(0, 255, 255),
        osbr::RGB(255, 0, 0),
        osbr::RGB(128, 0, 255),
        osbr::RGB(255, 0, 255),
        osbr::RGB(0, 255, 128),
        osbr::RGB(255, 255, 0)
    };
    data.color = colors[0];

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
                mode.nextMode();
            }
            else if (e.key.keysym.sym == SDLK_i) {
                bladeOn = !bladeOn;
            }
			else if (e.key.keysym.sym == SDLK_p) {
                data.mVolts += 100;
                if (data.mVolts > 4200)
                    data.mVolts = 3300;
                printf("mVolts=%d\n", data.mVolts);
			}
			else if (e.key.keysym.sym == SDLK_v) {
				data.volume = (data.volume + 1) % 5;
			}
			else if (e.key.keysym.sym == SDLK_o) {
                renderMode++;
                if (renderMode == NUM_RENDER_MODE)
                    renderMode = 0;
			}
			else if (e.key.keysym.sym == SDLK_c) {
				palette = (palette + 1) % 8;
                data.palette = palette;
                data.color = colors[palette];
				data.fontName = FONT_NAMES[palette];
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
            dotstarUI.Draw(dotstar4, 4, mode.mode(), bladeOn, data);
            dotstarUI.Draw(dotstar6, 6, mode.mode(), bladeOn, data);
		}

		const SDL_Rect src = { 0, 0, WIDTH, HEIGHT };
		SDL_Rect winRect;
		SDL_GetWindowSize(win, &winRect.w, &winRect.h);
		const int w = WIDTH * scale;
		const int h = HEIGHT * scale;
		SDL_Rect dst = { (winRect.w - w) / 2, (winRect.h - h) / 2, w, h };

        if (renderMode == RENDER_OLED) {
            oled.Commit();
        }
        else if (renderMode == RENDER_MATRIX) {
            oled.CommitFrom5x7(pixel75.Pixels());
        }
        else if (renderMode == RENDER_DOTSTAR_4) {
            oled.CommitFromDotstar(dotstar4, 4);
        }
        else if (renderMode == RENDER_DOTSTAR_6) {
            oled.CommitFromDotstar(dotstar6, 6);
        }

		SDL_RenderClear(ren);
		SDL_UpdateTexture(texture, NULL, oled.Pixels(), WIDTH * 4);
		SDL_RenderCopy(ren, texture, &src, &dst);
		SDL_RenderPresent(ren);
	}
	SDL_Quit();
	return 0;
}
