#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>

typedef unsigned char byte;

#define SUCCESS 1
#define FAIL 0

typedef struct
{
    char **words;
    int len;
} splitted_words;

#endif