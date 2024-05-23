#ifndef MISC_H
#define MISC_H

#include "types.h"

char *ptr_to_str(void *ptr);
void *str_to_ptr(const char *str);

splitted_words words_alloc(int len);
void words_free(splitted_words *spl_w);
void print_words(splitted_words spl_w);

int isnewline(int c);
int count_spaces(char *str);

char *strtok_wq(char *_str);

#endif