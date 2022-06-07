#include "renderer.h"
#include "oledsim.h"
#include "assets.h"
#include "sketcher.h"
#include "bladecolor.h"
#include "saberUtil.h"

#include "uiDotStar.h"
#include "uiPixel75.h"

#include "../saber/unittest.h"
#include "../saber/rgb.h"
#include "../saber/vrender.h"
#include "../saber/vectorui.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <chrono>

#define MONO_128_32 1

#ifdef MONO_128_32
static const int WIDTH = 128;
static const int HEIGHT = 32;
static const int OLED_WIDTH_SHIFT = 7;

#endif

class QuickProfile
{
public:
    QuickProfile(const char* name, bool print=true) {
        startTime = std::chrono::high_resolution_clock::now();
        this->name = name;
        this->print = print;
    }

    ~QuickProfile() {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::microseconds micro = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - startTime
            );
        if (print) printf("%s: %d micro-secondes\n", name, (int)micro.count());
    }

private:
    std::chrono::steady_clock::time_point startTime;
    const char* name;
    bool print;
};

osbr::RGBA* blockDrawRGBABuffer = 0;
uint8_t* blockDrawOLEDBUffer = 0;

void BlockDrawRGB(const BlockDrawChunk* chunk, int y, int n)
{
    for (int i = 0; i < n; ++i, ++chunk) {
        uint32_t* pixels = ((uint32_t*)blockDrawRGBABuffer) + y * WIDTH + chunk->x0;

        int n = chunk->x1 - chunk->x0;
        while (n--) {
            *pixels++ = chunk->color ? 0xffffffff : 0;
        }
    }
}

void BlockDrawOLED(const BlockDrawChunk* chunks, int n)
{
    // The renderer assumes clear to black beforehand.
    // But does still support drawing in black.
    for (int i = 0; i < n; ++i) {
        const BlockDrawChunk& chunk = chunks[i];

        // Simple for the single row.
        int row = chunk.y >> 3;
        int bit = chunk.y - (row << 3);
        uint8_t mask = 1 << bit;
        uint8_t* p = blockDrawOLEDBUffer + (row << OLED_WIDTH_SHIFT) + chunk.x0;

        if (chunk.color) {
            for (int nPix = chunk.x1 - chunk.x0; nPix > 0; nPix--, p++) {
                *p |= mask;
            }
}
        else {
            for (int nPix = chunk.x1 - chunk.x0; nPix > 0; nPix--, p++) {
                *p &= ~mask;
            }
        }
    }
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

#if false
    static const int NLIN = 64;
    for (int i = 0; i < (NLIN + 1); i++) {
        const double PI = 3.14159265359;
        // degrees:
        // double v = asin(i / 32.0) * 360.0 / (2.0 * PI);
        // printf("%f, ", v);
        // ISINE_ONE
        double v = asin(i / (double)NLIN) * ISINE_360 / (2.0 * PI);
        printf("%d, ", (int)(v + 0.5));
    }
    printf("\n");
    exit(0);
#endif

#ifdef MONO_128_32
    SimDisplay simDisplay(WIDTH, HEIGHT, 1);
    uint8_t* displayBuffer = new uint8_t[WIDTH*HEIGHT/8];
    
    VRender vrender;
    Renderer bRender;
    VectorUI vectorUI;

    blockDrawOLEDBUffer = displayBuffer;
    vrender.Attach(BlockDrawOLED);
    vrender.SetSize(WIDTH, HEIGHT);
    vrender.SetClip(VRender::Rect(0, 0, WIDTH, HEIGHT));
    vrender.Clear();

    bRender.Attach(WIDTH, HEIGHT, displayBuffer);
    //vrender.SetCamera(WIDTH, HEIGHT, -1, -1);
#endif

    // VRender can directly write to the display buffer
    Pixel75UI pixel75;
    osbr::RGBA dotstar1[1];
    osbr::RGBA dotstar4[4];
    osbr::RGBA dotstar6[6];
    DotStarUI dotstarUI(false);

	SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);

	UIRenderData data;
	data.volume = 2;
	data.mVolts = 3850;
	data.fontName = "Bespin";
    data.soundBank = 0;

    runUnitTests();
    
	SDL_Event e;
	int scale = 4;
	UIModeUtil mode(false);
	bool bladeOn = false;
	int count = 0;
	uint32_t lastUpdate = SDL_GetTicks();
    bool firstRender = true;
    BladeColor bladeColor;

    enum {
        RENDER_OLED,
        RENDER_MATRIX,
        RENDER_DOTSTAR_1,
        RENDER_DOTSTAR_4,
        RENDER_DOTSTAR_6,
        NUM_RENDER_MODE
    };
	int renderMode = RENDER_OLED;
    mode.set(UIMode::NORMAL);

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
    bladeColor.setBladeColor(data.color);
    bladeColor.setImpactColor(ColorRotated(data.color, 180));

	int palette = 0;

	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
            firstRender = true;
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
                bladeColor.setBladeColor(data.color);
                bladeColor.setImpactColor(ColorRotated(data.color, 180));
                data.fontName = FONT_NAMES[palette];
                data.soundBank = palette % 4;
			}
            else if (e.key.keysym.sym == SDLK_f) {
                bladeColor.doFlash(SDL_GetTicks());
            }
		}

		uint32_t t = SDL_GetTicks();
        bladeColor.tick(t);
		if (t - lastUpdate > 50) {
            uint32_t delta = t - lastUpdate;
			lastUpdate = t;
			uint8_t value = int(127.8 * (sin(count * 0.2) + 1.0));
			++count;

#ifdef MONO_128_32

#if false    // Test the DrawTestData
            float s = sinf(t * 0.002f) + 1.0f;
            vectorUI.PushTestData(s, 0.2f, 1.8f, t, 1.0f);
            bladeOn = true;
#endif

            memset(displayBuffer, 0, WIDTH * HEIGHT / 8);
            vrender.Clear();
            vectorUI.Draw(&vrender, &bRender, t, mode.mode(), bladeOn, &data);
            vrender.ClearTransform();
#endif
			pixel75.Process(mode.mode(), data);
            dotstarUI.Process(dotstar1, 1, 255, t, mode.mode(), bladeOn, data);
            dotstarUI.Process(dotstar4, 4, 255, t, mode.mode(), bladeOn, data);
            dotstarUI.Process(dotstar6, 6, 255, t, mode.mode(), bladeOn, data);
		}

		const SDL_Rect src = { 0, 0, WIDTH, HEIGHT };
		SDL_Rect winRect;
		SDL_GetWindowSize(win, &winRect.w, &winRect.h);
		const int w = WIDTH * scale;
		const int h = HEIGHT * scale;
		SDL_Rect dst = { (winRect.w - w) / 2, (winRect.h - h) / 2, w, h };

        SDL_RenderClear(ren);
#ifdef MONO_128_32
        if (renderMode == RENDER_OLED) {
            simDisplay.CommitFromBuffer(displayBuffer, WIDTH, HEIGHT);
        }
        else if (renderMode == RENDER_MATRIX) {
            simDisplay.CommitFrom5x7(pixel75.Pixels());
        }
        else if (renderMode == RENDER_DOTSTAR_1) {
            simDisplay.CommitFromDotstar(dotstar1, 1, &bladeColor.getColor());
        }
        else if (renderMode == RENDER_DOTSTAR_4) {
            simDisplay.CommitFromDotstar(dotstar4, 4, &bladeColor.getColor());
        }
        else if (renderMode == RENDER_DOTSTAR_6) {
            simDisplay.CommitFromDotstar(dotstar6, 6, &bladeColor.getColor());
        }
        SDL_UpdateTexture(texture, NULL, simDisplay.Pixels(), WIDTH * 4);
#endif
		SDL_RenderCopy(ren, texture, &src, &dst);
		SDL_RenderPresent(ren);
	}
    delete[] displayBuffer;
	SDL_Quit();
	return 0;
}
