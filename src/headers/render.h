#ifndef RENDER_H
#define RENDER_H

#include "types.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define BYTES_PER_PIXEL 4
#define DEPTH BYTES_PER_PIXEL * 8

typedef struct
{
    SDL_Texture *ptr;
    char *name;
    int width, height;
} texture;

typedef struct
{
    TTF_Font *ptr;
    char *name;
    int size;
} font;

void init_all(int width, int height, SDL_Window **win, SDL_Renderer **renderer);

void exit_all(SDL_Window *win, SDL_Renderer *renderer);

#endif