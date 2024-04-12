#include "headers/script_parser.h"

// reads line from config file and returns bunch of tokens, ready to be parsed
// reallocates values if needed

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
        return -1;
    }

    hash_table **vars = alloc_table();
    if (!vars)
    {
        fclose(f);
        perror("Error allocating memory for vars");
        return -1;
    }

    hash_table **resources = alloc_table();
    if (!resources)
    {
        fclose(f);
        perror("Error allocating memory for resources");
        return -1;
    }

    splitted_words command = {0, 0};

    const int total_commands = 13;

    enum CMD_TYPE
    {
        CMD_UNKNOWN = -1, //
        CMD_EMPTY,        //
        CMD_END,          // variable control commands
        CMD_SET,          //
        CMD_REMOVE,       //
        CMD_APPEND,       //
        CMD_PRINT_VARS,   //
        CMD_INIT_WINDOW,  // sdl2 and rendering related commands
        CMD_LOAD_IMAGE,   //
        CMD_SET_COLOR,    //
        CMD_RENDER_IMAGE, //
        CMD_RENDER_POINT, //
        CMD_RENDER_LINE,  //
        CMD_RENDER_RECT   //
    };

    cmd_syntax syntax_arr[total_commands];

    syntax_arr[CMD_EMPTY] = make_new_entry("empty", "empty line idk", CMD_EMPTY, 0);
    syntax_arr[CMD_END] = make_new_entry("end", "end command to end the script i guess", CMD_END, 0);
    syntax_arr[CMD_SET] = make_new_entry("set", "sets a value for internal variable, first arg for variable name and second for its value", CMD_SET, 2, STRING, STRING);
    syntax_arr[CMD_REMOVE] = make_new_entry("rem", "removes a variabl", CMD_REMOVE, 1, STRING);
    syntax_arr[CMD_APPEND] = make_new_entry("append", "may concatenate a string to a variable", CMD_APPEND, 2, STRING, STRING);
    syntax_arr[CMD_PRINT_VARS] = make_new_entry("print_vars", "prints current variables", CMD_PRINT_VARS, 0);
    syntax_arr[CMD_INIT_WINDOW] = make_new_entry("init_window", "grrrr", CMD_INIT_WINDOW, 3, STRING, INT, INT);
    syntax_arr[CMD_LOAD_IMAGE] = make_new_entry("load_image", "loads image from your disk, single argument", CMD_LOAD_IMAGE, 1, STRING);
    syntax_arr[CMD_SET_COLOR] = make_new_entry("set_color", "select a color for render_point, render_line and so, 4 numbers for RGBA", CMD_SET_COLOR, 4, INT, INT, INT, INT);
    syntax_arr[CMD_RENDER_IMAGE] = make_new_entry("render_image", "copies an imeg to specified coordinates on ascreen, 1st arg as a name of file and other 4 for xy coordinates and width&height", CMD_RENDER_IMAGE, 5, STRING, INT, INT, INT, INT);
    syntax_arr[CMD_RENDER_POINT] = make_new_entry("render_point", "rendrs point on xy coordinates", CMD_RENDER_POINT, 2, INT, INT);
    syntax_arr[CMD_RENDER_LINE] = make_new_entry("render_line", "renders a line between 2 points", CMD_RENDER_LINE, 4, INT, INT, INT, INT);
    syntax_arr[CMD_RENDER_RECT] = make_new_entry("render_rect", "rneders rect betwen 2 points", CMD_RENDER_RECT, 4, INT, INT, INT, INT);

    char *out_filename = 0;

    int width, height;

    int parse_more = 1;
    int initialized = 0;

    SDL_Window *win = 0;
    SDL_Renderer *renderer = 0;

    for (int current_line = 0; parse_more == 1; current_line++)
    {
        if (!read_config_line(f, &command))
            break;

        int cmd_id = eval_id_of_command(command, syntax_arr, total_commands);

        if (cmd_id == CMD_UNKNOWN)
        {
            fprintf(stderr, "Unknown command on line %d: ", current_line);
            print_words(command);
            continue;
        }

        if (cmd_id == CMD_EMPTY)
            continue;

        if (!is_syntax_good(command, syntax_arr[cmd_id]))
        {
            fprintf(stderr, "Syntax error on line %d: ", current_line);
            print_words(command);
            continue;
        }

        switch (cmd_id)
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
        case CMD_PRINT_VARS:
            print_table(vars);
            break;
        case CMD_INIT_WINDOW:
            if (initialized)
                break;
            out_filename = malloc(strlen(command.words[1]));
            strcpy(out_filename, command.words[1]);
            width = atoi(command.words[2]);
            height = atoi(command.words[3]);
            init_all(width, height, &win, &renderer);
            SDL_RenderClear(renderer);
            initialized = 1;
            break;
        case CMD_SET_COLOR:
            SDL_SetRenderDrawColor(renderer, atoi(command.words[1]), atoi(command.words[2]), atoi(command.words[3]), atoi(command.words[4]));
            break;
        case CMD_RENDER_POINT:
            SDL_RenderDrawPoint(renderer, atoi(command.words[1]), atoi(command.words[2]));
            break;
        case CMD_RENDER_LINE:
            SDL_RenderDrawLine(renderer, atoi(command.words[1]), atoi(command.words[2]), atoi(command.words[3]), atoi(command.words[4]));
            break;
        case CMD_LOAD_IMAGE:
            char *tmp = ptr_to_str(IMG_LoadTexture(renderer, command.words[1]));
            put_entry(resources, command.words[1], tmp);
            free(tmp);
            break;
        case CMD_RENDER_IMAGE:
            SDL_Rect dest = {atoi(command.words[2]), atoi(command.words[3]), atoi(command.words[4]), atoi(command.words[5])};
            SDL_RenderCopy(renderer, str_to_ptr(get_entry(resources, command.words[1])), 0, &dest);
            break;
        default:
            break;
        }

        if (!initialized)
        {
            fprintf(stderr, "Window wasnt initialized, please specify window sizes with this command on your first line:\n\tinit_window <out_filename.png> <width> <height>\n");
            return -1;
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
        exit_all(win, renderer);

    // Free allocated memory and close file
    free_table(vars);
    free_table(resources);
    fclose(f);

    return 0;
}