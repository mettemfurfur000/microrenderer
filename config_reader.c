#include "hash_table.c"
#include "config_command_args.c"
#include "render.c"
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

    cmd_syntax syntax_arr[] = {
        {"empty", 0, 0},
        {"end", 0, 0},
        {"var_set", 2, {STRING, STRING}},
        {"var_rem", 1, {STRING}},
        {"var_add", 2, {STRING, STRING}},
        {"init_window", 3, {STRING, INT, INT}},
        {"load_image", 2, {STRING, STRING}},
        {"set_color", 3, {INT, INT, INT}},
        {"render_image", 3, {STRING, INT, INT}},
        {"render_point", 2, {INT, INT}},
        {"render_line", 4, {INT, INT, INT, INT}},
        {"render_rect", 4, {INT, INT, INT, INT}}

    };

    char *out_filename = 0;

    int width, height;

    int parse_more = 1;
    int current_line = 0;
    int initialized = 0;

    while (parse_more)
    {
        if (!read_config_line(f, &command))
            break;

        int cmd_type = eval_type_of_command(command);
        int syntax_is_bad = check_command_syntax(command, syntax_arr[cmd_type + 1]);

        if (syntax_is_bad)
            continue;

        switch (cmd_type)
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
            SDL_RenderClear(renderer);
            initialized = 1;
            break;
        case CMD_SET_COLOR:
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            break;
        case CMD_RENDER_POINT:
            SDL_RenderDrawPoint(renderer, atoi(command.words[1]), atoi(command.words[2]));
        default:
            // Handle unrecognized command
            fprintf(stderr, "Unrecognized command:\n");
            print_words(command);
            break;
        }

        current_line++;

        if (!initialized)
        {
            fprintf(stderr, "Window wasnt initialized, please specify window sizes with this command on your first line:\n\tinit_window <out_filename.png> <width> <height>\n");
            break;
        }
    }

    // Write results to a file
    SDL_Surface *surface;

    if (!(surface = SDL_CreateRGBSurface(0, width, height, DEPTH, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff)))
    {
        printf("SDL_CreateRGBSurface: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_RenderReadPixels(renderer, 0, SDL_PIXELFORMAT_RGBA8888, surface->pixels, BYTES_PER_PIXEL * width);
    IMG_SavePNG(surface, out_filename); // Output buffer
    SDL_FreeSurface(surface);

    if (initialized)
        exit_all();

    // Free allocated memory and close file
    free_table(vars);
    free_table(resources);
    fclose(f);

    return SUCCESS;
}