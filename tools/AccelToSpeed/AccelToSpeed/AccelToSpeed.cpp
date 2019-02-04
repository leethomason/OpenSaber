#include <SDL.h>
#include <assert.h>

#include <iostream>

static const int WIDTH = 1024;
static const int HEIGHT = 768;

int main(int argc, char* argv[])
{
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

    SDL_Event e;

    while (true) {
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) {
            break;
        }
        SDL_RenderClear(ren);
        //SDL_UpdateTexture(texture, NULL, oled.Pixels(), WIDTH * 4);
        SDL_RenderCopy(ren, texture, 0, 0);
        SDL_RenderPresent(ren);
    }
    SDL_Quit();
    return 0;
}
