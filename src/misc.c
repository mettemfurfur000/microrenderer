#include "headers/misc.h"

char *ptr_to_str(void *ptr)
{
    static char internal_buffer[32];

    sprintf(internal_buffer, "%p", (void *)ptr);
    return internal_buffer;
}

void *str_to_ptr(const char *str)
{
    if (!str)
        return 0;
    uintptr_t address;
    sscanf(str, "%p", (void **)&address);
    void *ptr = (void *)address;
    return ptr;
}

splitted_words words_alloc(int len)
{
    splitted_words g;
    g.words = (char **)malloc(len * sizeof(void *));
    g.len = len;
    return g;
}

void words_free(splitted_words *spl_w)
{
    for (int i = 0; i < spl_w->len; i++)
        free(spl_w->words[i]);
    free(spl_w->words);
    spl_w->words = 0;
}

void print_words(splitted_words spl_w)
{
    for (int i = 0; i < spl_w.len; i++)
        printf("[%s] ", spl_w.words[i]);
    printf(".\n");
}

int isnewline(int c)
{
    if (c == '\r' || c == '\n')
        return 1;
    return 0;
}

int count_spaces(char *str)
{
    int spaces = 0;
    while (!isnewline(*str))
    {
        // skips doulbe-quoted sections
        if (*str == '"')
        {
            str++;
            while (*str != '"')
                str++;
            str++;
            continue;
        }

        while (!isspace(*str) && !isnewline(*str) && *str != '\0')
            str++;

        if (isnewline(*str) || *str == '\0')
            break;

        spaces++;

        while (isspace(*str))
            str++;
    }
    return spaces;
}

// litle bit destructive
char *strtok_wq(char *_str)
{
    static char *end_of_last_word = 0;
    if (!end_of_last_word && !_str)
        return 0;
    if (_str && *_str == '\0')
        return 0;
    char *str = _str ? _str : end_of_last_word;
    while (isspace(*str))
        str++;
    char *start_of_word = str;
    for (;;)
    {
        if (isspace(*str))
            break;
        if (*str == 0)
            break;
        if (*str == '"')
        {
            str++;
            while (*str != '"')
                str++;
            str++;
            continue;
        }
        str++;
    }
    end_of_last_word = (*str == '\0' ? 0 : str + 1);
    *str = '\0';

    if (*start_of_word == '"')
        start_of_word++;
    if (*(str - 1) == '"')
        *(str - 1) = '\0';
    return start_of_word;
}