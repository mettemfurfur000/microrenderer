#ifndef TYPES
#define TYPES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "vec/src/vec.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef unsigned char byte;

#define SUCCESS 1
#define FAIL 0

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

typedef struct
{
    char **words;
    int len;
} splitted_words;

#endif