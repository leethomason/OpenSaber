#define _CRT_SECURE_NO_WARNINGS
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <algorithm>
#include <vector>
#include <assert.h>
#include <stdint.h>

#include "accelspeed.h"

static const int WIDTH = 1024;
static const int HEIGHT = 512;

struct float3 {
    float x, y, z;
};

struct AccelData {
    float3 accel;
    float g;
    uint32_t time;
};

struct RGBA {
    uint8_t r, g, b, a;
};

static const RGBA RED = { 255, 0, 0, 255 };
static const RGBA GREEN = { 0, 255, 0, 255 };
static const bool CLIP = true;

int main(int argc, char* argv[])
{
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

    FILE* fp = fopen("accel_cap.txt", "r");
    if (!fp) {
        printf("Could not apen accel_cap\n");
        SDL_Quit();
        return 1;
    }
    char buf[256];
    std::vector<AccelData> data;
    std::vector<float> speeds;
    uint32_t t0 = UINT32_MAX, t1 = 0;
    float gMax = 0, gMin = 16.0f;
    float speedMax = 0, speedMin = 100.0;

    while (fgets(buf, 255, fp)) {
        AccelData ad;
        const char* p = strchr(buf, '=') + 1;
        sscanf(p, "%f", &ad.accel.x);
        p = strchr(p + 1, '=') + 1;
        sscanf(p, "%f", &ad.accel.y);
        p = strchr(p + 1, '=') + 1;
        sscanf(p, "%f", &ad.accel.z);
        p = strchr(p + 1, '=') + 1;
        sscanf(p, "%f", &ad.g);
        assert(ad.g > 0);
        p = strchr(p + 1, '=') + 1;
        sscanf(p, "%d", &ad.time);

        data.push_back(ad);
        t0 = std::min(t0, ad.time);
        t1 = std::max(t1, ad.time);
        gMax = std::max(gMax, ad.g);
        gMin = std::min(gMin, ad.g);
    }
    fclose(fp);

    RGBA* pixels = new RGBA[WIDTH*HEIGHT];
    memset(pixels, 0, sizeof(RGBA)*WIDTH*HEIGHT);
    AccelSpeed accelSpeed;

    for (size_t i = 0; i < data.size(); ++i) {
        const AccelData& ad = data[i];
        int x = WIDTH * (ad.time - t0) / (t1 - t0);
        int y = int(HEIGHT * (ad.g - gMin) / (gMax - gMin));
        if (x >= WIDTH) x = WIDTH - 1;
        if (y >= HEIGHT) y = HEIGHT - 1;

        assert(x >= 0 && x < WIDTH);
        assert(y >= 0 && y < HEIGHT);

        pixels[(HEIGHT - y - 1)*WIDTH + x] = RED;
   
        if (i > 0) {
            uint32_t micro = (data[i].time - data[i - 1].time) * 1000;
            accelSpeed.push(data[i].accel.x, data[i].accel.y, data[i].accel.z, micro);

            speeds.push_back(accelSpeed.speed());
            speedMin = std::min(speedMin, accelSpeed.speed());
            speedMax = std::max(speedMax, accelSpeed.speed());
        }
    }

    for (size_t i = 0; i < speeds.size(); ++i) {
        const AccelData& ad = data[i];
        int x = WIDTH * (ad.time - t0) / (t1 - t0);
        int y = int(HEIGHT * (speeds[i] - speedMin) / (speedMax -speedMin));
        if (x >= WIDTH) x = WIDTH - 1;
        if (y >= HEIGHT) y = HEIGHT - 1;

        pixels[(HEIGHT - y - 1)*WIDTH + x].g = 0xff;
        pixels[(HEIGHT - y - 1)*WIDTH + x].a = 0xff;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    assert(ren);

    SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
    assert(texture);

    SDL_Event e;

    while (true) {
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) {
            break;
        }
        SDL_RenderClear(ren);
        SDL_UpdateTexture(texture, 0, pixels, WIDTH*sizeof(RGBA));
        SDL_RenderCopy(ren, texture, 0, 0);
        SDL_RenderPresent(ren);
    }
    SDL_Quit();
    return 0;
}
