#include "hash_table.c"
#include "types.h"

char *ptr_to_str(void *ptr)
{
    uintptr_t address = (uintptr_t)ptr;
    char *str = (char *)malloc(sizeof(char) * 20); // Assuming maximum address length of 20 characters
    sprintf(str, "%p", (void *)address);
    return str;
}

void *str_to_ptr(const char *str)
{
    uintptr_t address;
    sscanf(str, "%p", (void **)&address);
    void *ptr = (void *)address;
    return ptr;
}

splitted_words words_alloc(int len)
{
    splitted_words g;
    g.words = (char **)malloc(len * sizeof(void *));
    return g;
}

void words_free(splitted_words spl_w)
{
    for (int i = 0; i < spl_w.len; i++)
        free(spl_w.words[i]);
    free(spl_w.words);
    spl_w.words = 0;
}

void print_words(splitted_words spl_w)
{
    for (int i = 0; i < spl_w.len; i++)
        printf("[%s] ", spl_w.words[i]);
    printf(".\n");
}

int isnewline(int c)
{
    switch (c)
    {
    case '\n':
        return 1;
    case '\r':
        return 1;
    default:
        return 0;
    }
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

        if (isnewline(*str))
            spaces--;
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

// reads line from config file and returns bunch of tokens, ready to be parsed in the future
// allocates array of pointers on *out
int read_config_line(FILE *f, splitted_words *spl_w)
{
    char buffer[512] = {0};

    if (spl_w->words != 0)
        words_free(*spl_w);

    if (fgets(buffer, sizeof(buffer), f))
    {
        int words = count_spaces(buffer) + 1;

        *spl_w = words_alloc(words);

        char *token = strtok_wq(buffer);
        for (int i = 0; i < words; i++)
        {
            spl_w->words[i] = (char *)malloc(strlen(token) + 1);
            strcpy(spl_w->words[i], token);
            token = strtok_wq(0);
        }

        spl_w->len = words;
    }

    if (feof(f))
        return FAIL;

    return SUCCESS;
}

enum CMD_TYPE
{
    CMD_UNKNOWN = 0,  //
    CMD_EMPTY,        //
    CMD_END,          // variable control commands
    CMD_SET,          //
    CMD_REMOVE,       //
    CMD_APPEND,       //
    CMD_INIT_WINDOW,  // sdl2 and rendering related commands
    CMD_LOAD_IMAGE,   //
    CMD_SET_COLOR,    //
    CMD_RENDER_IMAGE, //
    CMD_RENDER_POINT, //
    CMD_RENDER_LINE,  //
    CMD_RENDER_RECT   //
} CMD_TYPE;

int eval_type_of_command(splitted_words w)
{
    if (w.len == 0)
        return CMD_EMPTY;
    if (strcmp(w.words[0], "end") == 0)
        return CMD_END;
    if (strcmp(w.words[0], "var_set") == 0)
        return CMD_SET;
    if (strcmp(w.words[0], "var_rem") == 0)
        return CMD_REMOVE;
    if (strcmp(w.words[0], "var_add") == 0)
        return CMD_APPEND;
    if (strcmp(w.words[0], "init_window") == 0)
        return CMD_INIT_WINDOW;
    if (strcmp(w.words[0], "load_image") == 0)
        return CMD_LOAD_IMAGE;
    if (strcmp(w.words[0], "set_color") == 0)
        return CMD_SET_COLOR;
    if (strcmp(w.words[0], "render_image") == 0)
        return CMD_RENDER_IMAGE;
    if (strcmp(w.words[0], "render_point") == 0)
        return CMD_RENDER_POINT;
    if (strcmp(w.words[0], "render_line") == 0)
        return CMD_RENDER_LINE;
    if (strcmp(w.words[0], "render_rect") == 0)
        return CMD_RENDER_RECT;

    return CMD_UNKNOWN;
}

int parse_config(char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        perror("Error opening file");
        return FAIL;
    }

    hash_table **vars = alloc_table();
    if (!vars)
    {
        fclose(f);
        perror("Error allocating memory for vars");
        return FAIL;
    }

    hash_table **resources = alloc_table();
    if (!resources)
    {
        fclose(f);
        perror("Error allocating memory for resources");
        return FAIL;
    }

    splitted_words command = {0, 0};

    char *out_filename = 0;

    int width, height;

    int parse_more = 1;
    int current_line = 0;
    int initialized = 0;
    while (parse_more)
    {
        if (!read_config_line(f, &command))
            break;

        switch (eval_type_of_command(command))
        {
        case CMD_END:
            parse_more = 0;
            break;
        case CMD_SET:
        CMD_SET_SHORTCUT:
            put_entry(vars, command.words[1], command.words[2]);
            break;
        case CMD_REMOVE:
            remove_entry(vars, command.words[1]);
            break;
        case CMD_APPEND:
            char *value = get_entry(vars, command.words[1]);
            if (!value)
            {
                fprintf(stderr, "Entry '%s' does not exist, creating value instead\n", command.words[1]);
                goto CMD_SET_SHORTCUT;
            }
            // Allocate memory for the new value
            size_t new_len = strlen(value) + strlen(command.words[2]) + 1;
            char *new_value = (char *)malloc(new_len);
            if (!new_value)
            {
                // Handle memory allocation failure
                perror("Error allocating memory for new_value");
                break;
            }
            strcpy(new_value, value);
            strcat(new_value, command.words[2]);
            put_entry(vars, command.words[1], new_value);
            free(new_value); // Remember to free the allocated memory
            break;
        case CMD_INIT_WINDOW:
            if (initialized)
                break;
            out_filename = malloc(strlen(command.words[1]));
            strcpy(out_filename, command.words[1]);
            width = atoi(command.words[2]);
            height = atoi(command.words[3]);
            init_all(width, height);
            initialized = 1;
            break;
        default:
            // Handle unrecognized command
            fprintf(stderr, "Unrecognized command:\n");
            print_words(command);
            break;
        }

        current_line++;

        if (!initialized)
        {
            fprintf(stderr, "Window wasnt initialized, please specify window sizes with this command on your first line:\n\tinit_window <out_filename> <width> <height>\n");
            break;
        }
    }

    if (initialized)
        exit_all();

    // Free allocated memory and close file
    free_table(vars);
    free_table(resources);
    fclose(f);

    return SUCCESS;
}