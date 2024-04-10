#include "types.h"

const int BYTES_PER_PIXEL = 4;
const int DEPTH = BYTES_PER_PIXEL * 8;

SDL_Window *win;
SDL_Renderer *renderer;

void init_all(int width, int height)
{
    SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "0");
    SDL_Init(SDL_INIT_VIDEO);

    if (TTF_Init())
    {
        printf("%s\n", TTF_GetError());
        exit(1);
    }

    putenv("SDL_VIDEODRIVER=dummy");

    if (!(win = SDL_CreateWindow("Microrenderer framebuffer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_HIDDEN)))
    {
        printf("SDL_CreateWindow: %s\n", SDL_GetError());
        exit(1);
    }

    if (!(renderer = SDL_CreateRenderer(win, -1, 0)))
    {
        printf("SDL_CreateRenderer: %s\n", SDL_GetError());
        exit(1);
    }
}

void exit_all()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
}