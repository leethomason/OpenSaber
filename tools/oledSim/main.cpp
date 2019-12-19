#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <chrono>

#include "renderer.h"
#include "oledsim.h"
#include "assets.h"
#include "sketcher.h"
#include "../saber/unittest.h"
#include "../saber/rgb.h"
#include "../saber/vrender.h"
#include "../saber/vectorui.h"

#define MONO_128_32 1
//#define RGB_160_80 2

#ifdef MONO_128_32
static const int WIDTH = 128;
static const int HEIGHT = 32;
#define USE_VRENDER

#endif
#ifdef RGB_160_80
static const int WIDTH = 160;
static const int HEIGHT = 80;
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
#ifdef VECTOR_MONO
        uint32_t c = chunk->rgb ? 0xffffffff : 0;
#else
        uint32_t c = (chunk->rgb.r << 24) | (chunk->rgb.g << 16) | (chunk->rgb.b << 8) | 0xff;
#endif
        uint32_t* pixels = ((uint32_t*)blockDrawRGBABuffer) + y * WIDTH + chunk->x0;

        int n = chunk->x1 - chunk->x0;
        while (n--) {
            *pixels++ = c;
        }
    }
}

void BlockDrawOLED(const BlockDrawChunk* chunks, int n)
{
#ifdef PRECOMPUTE_MASK
    static const uint8_t MASK0[] = { 1, 3, 7, 15, 31, 63, 127, 255 };
    static const uint8_t MASK1[] = { 2, 6, 14, 30, 62, 126, 254 };
    static const uint8_t MASK2[] = { 4, 12, 28, 60, 124, 252 };
    static const uint8_t MASK3[] = { 8, 24, 56, 120, 248 };
    static const uint8_t MASK4[] = { 16, 48, 112, 240 };
    static const uint8_t MASK5[] = { 32, 96, 224 };
    static const uint8_t MASK6[] = { 64, 192 };
    static const uint8_t MASK7[] = { 128 };
    static const uint8_t* MASK[] = {
        MASK0, MASK1, MASK2, MASK3, MASK4, MASK5, MASK6, MASK7
    };
#endif

    // Clear to black beforehand; don't need to set black runs.
    for (int i = 0; i < n; ++i) {
        const BlockDrawChunk& chunk = chunks[i];

#ifdef VECTOR_MONO
        if (chunk.rgb == 0) continue;
#else
        if (chunk.rgb.get() == 0) continue;
#endif
        // Simple for the single row.
        int row = chunk.y >> 3;
        int bit = chunk.y - (row << 3);
        uint8_t mask = 1 << bit;
        uint8_t* p = blockDrawOLEDBUffer + row * WIDTH + chunk.x0;
        for (int nPix = chunk.x1 - chunk.x0; nPix > 0; nPix--, p++) {
            *p |= mask;
        }
    }
}


UIModeUtil::UIModeUtil()
{
    lastActive = 0;
}

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

#ifdef MONO_128_32
    SimDisplay simDisplay(WIDTH, HEIGHT, 1);
    uint8_t* displayBuffer = new uint8_t[WIDTH*HEIGHT/8];
    
#   ifdef USE_VRENDER
    VRender vrender;

    blockDrawOLEDBUffer = displayBuffer;
    vrender.Attach(BlockDrawOLED);
    vrender.SetClip(VRender::Rect(0, 0, WIDTH, HEIGHT));
    vrender.SetSize(WIDTH, HEIGHT);
    //vrender.SetCamera(WIDTH, HEIGHT, -1, -1);

#   else
    memset(displayBuffer, 0, WIDTH * HEIGHT);
	Renderer renderer;
    renderer.Attach(WIDTH, HEIGHT, displayBuffer);
#   endif
#endif
#ifdef RGB_160_80

    // VRender can directly write to the display buffer
    uint32_t* displayBuffer = new uint32_t[WIDTH*HEIGHT];
    memset(displayBuffer, 0, 4 * WIDTH*HEIGHT);

    VRender vrender;
    blockDrawRGBABuffer = (osbr::RGBA*)displayBuffer;
    vrender.Attach(BlockDrawRGB);
    vrender.SetSize(WIDTH, HEIGHT);
    vrender.ClearClip();
    
    vrender.DrawRect(10, 10, WIDTH-20, HEIGHT-20, osbr::RGBA(255, 255, 255, 64));
    VRender::Vec2 points[4] = {
        { WIDTH/2, 0 }, {WIDTH, HEIGHT/2}, {WIDTH/2, HEIGHT}, {0,HEIGHT/2}
    };
    vrender.DrawPoly(points, 4, osbr::RGBA(255, 255, 255, 64));

    vrender.SetTransform(FixedNorm(4, 100), 0, 0);
    vrender.DrawRect(0, 0, 40, 40, osbr::RGBA(0, 255, 255, 192));
    vrender.DrawRect(40, 0, 40, 40, osbr::RGBA(255, 0, 255, 192));
    vrender.DrawRect(30, 10, 20, 20, osbr::RGBA(255, 255, 255, 100));

    vrender.ClearTransform();
    for (int r = 0; r < 8; ++r) {
        vrender.SetTransform(FixedNorm(r, 8), 3 * WIDTH / 4, HEIGHT / 2);
        vrender.DrawRect(25, -5, 10, 10, osbr::RGBA(0, 255, 0, 200));
    }
    vrender.ClearTransform();
    VRender::Vec2 p2[] = {
        {10, 70}, {20, 60}, {30, 70}, {20, 80}
    };
    int T = 1;
    VRender::Vec2 p2inner[] = {
        {10+T, 70}, {20, 60+T}, {30-T, 70}, {20, 80-T}
    };
    vrender.PushLayer();
    vrender.DrawPoly(p2, 4, osbr::RGBA(255, 255, 0));
    vrender.DrawPoly(p2inner, 4, osbr::RGBA(255, 255, 0));
    vrender.PopLayer();

    vrender.Render();
#endif

    Pixel_7_5_UI pixel75;
    osbr::RGB dotstar4[4];
    osbr::RGB dotstar6[6];
    DotStarUI dotstarUI;

	SDL_SetRenderDrawColor(ren, 128, 128, 128, 255);

	UIRenderData data;
	data.volume = 2;
	data.mVolts = 3850;
	data.fontName = "Bespin";

    runUnitTests();
    
	SDL_Event e;
	int scale = 4;
	UIModeUtil mode;
	bool bladeOn = false;
	int count = 0;
	uint32_t lastUpdate = SDL_GetTicks();
    bool firstRender = true;

    enum {
        RENDER_OLED,
        RENDER_MATRIX,
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
				data.fontName = FONT_NAMES[palette];
			}
		}

		uint32_t t = SDL_GetTicks();
		if (t - lastUpdate > 100) {
			lastUpdate = t;
			uint8_t value = int(127.8 * (sin(count * 0.2) + 1.0));
			++count;

#ifdef MONO_128_32
#   ifdef USE_VRENDER
            memset(displayBuffer, 0, WIDTH * HEIGHT / 8);
            vrender.Clear();
            VectorUI::Draw(&vrender, t, mode.mode(), bladeOn, &data);
            if (firstRender) {
                printf("Render: numEdges=%d/%d size=%d\n", vrender.NumEdges(), VRender::MAX_EDGES, (int)sizeof(VRender));
                firstRender = false;
            }
            vrender.Render();
#   else
            sketcher.Draw(&renderer, t, mode.mode(), bladeOn, &data);
#   endif
#endif
#ifdef RGB_160_80
#if true
            {
                // 88ms
                // Rasterize: 43%  -> SortActive 28
                //                    RasterizeLine 46
                // AddToColorStack 23
                // BlockDrawRGB 14
                // ------------------------
                // Optimize: separate ActiveEdge from edge.
                // 60ms (nice!)
                // AddToColorStack 36
                // Rasterize 31
                // BlockDrawRGB 21
                // -------------------------
                // 50ms
                // Optimize: better color stack
                // Rasterize 36 -> RasterizeLine 72
                // AddToColorStack 30
                // BlockDrawRGB 23
                // ---------------------
                // Some lower level optimization
                // AddToColorStack 30
                // BlockDrawRGB 21
                // RasterizeLine 18
                // Rasterize 16

                QuickProfile qp("multidraw", firstRender);
        //        for (int j = 0; j < 500; ++j) 
                {
                    for (int i = 0; i < WIDTH*HEIGHT; ++i) displayBuffer[i] = 0xff0000ff;
                    //vrender.SetClip(VRender::Rect(1, 1, WIDTH - 1, HEIGHT - 1));
                    VectorUI::Draw(&vrender, t, mode.mode(), bladeOn, &data);
                }
            }
            if (firstRender) {
                firstRender = false;
                printf("Sizeof VRender=%d\n", (int)sizeof(VRender));
                printf("NumEdges=%d\n", vrender.NumEdges());
            }
#endif
#endif
			pixel75.Draw(t, mode.mode(), bladeOn, &data);
            dotstarUI.Draw(dotstar4, 4, t, mode.mode(), bladeOn, data);
            dotstarUI.Draw(dotstar6, 6, t, mode.mode(), bladeOn, data);
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
        else if (renderMode == RENDER_DOTSTAR_4) {
            simDisplay.CommitFromDotstar(dotstar4, 4);
        }
        else if (renderMode == RENDER_DOTSTAR_6) {
            simDisplay.CommitFromDotstar(dotstar6, 6);
        }
        SDL_UpdateTexture(texture, NULL, simDisplay.Pixels(), WIDTH * 4);
#endif
#ifdef RGB_160_80
        SDL_UpdateTexture(texture, NULL, displayBuffer, WIDTH * 4);
#endif
		SDL_RenderCopy(ren, texture, &src, &dst);
		SDL_RenderPresent(ren);
	}
    delete[] displayBuffer;
	SDL_Quit();
	return 0;
}
