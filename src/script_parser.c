#include "headers/script_parser.h"
#include "headers/script_commands.h"
#include <string.h>
#include <stdlib.h>

char *strdup(const char *s)
{
    size_t slen = strlen(s);
    char *result = malloc(slen + 1);
    return result ? memcpy(result, s, slen + 1) : NULL;
}

// reads line from config file and returns bunch of tokens, ready to be parsed
// reallocates values if needed

int read_config_line(FILE *f, splitted_words *spl_w)
{
    char buffer[512] = {0};

    if (spl_w->words != 0)
        words_free(spl_w);

    if (fgets(buffer, sizeof(buffer), f))
    {
        if (isnewline(*buffer))
        {
            spl_w->len = 0;
            return SUCCESS;
        }

        int words = count_spaces(buffer) + 1;
        spl_w->len = words;

        *spl_w = words_alloc(words);

        char *token = strtok_wq(buffer);
        for (int i = 0; i < words; i++)
        {
            spl_w->words[i] = (char *)malloc(strlen(token) + 1);
            strcpy(spl_w->words[i], token);
            token = strtok_wq(0);
        }
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

    enum CMD_TYPE
    {
        CMD_UNKNOWN = -1, //
        CMD_EMPTY,        //
        CMD_END,          // variable control commands
        CMD_SET,          //
        CMD_REMOVE,       //
        CMD_APPEND,       //
        CMD_PRINT_VARS,   //
        CMD_INIT_WINDOW,  // sdl2 rendering related commands
        CMD_MAKE_SDL_OBJECT,
        CMD_LOAD_IMAGE,   //
        CMD_CROP_IMAGE,   //
        CMD_SET_COLOR,    //
        CMD_RENDER_IMAGE, //
        CMD_RENDER_POINT, //
        CMD_RENDER_LINE,  //
        CMD_RENDER_RECT,  //
        CMD_LAST
    };

#define total_commands CMD_LAST + 1

    cmd_syntax syntax_arr[total_commands] = {};

    syntax_arr[CMD_EMPTY] = make_new_entry("empty", "empty line idk", cmd_empty, CMD_EMPTY, 0);
    syntax_arr[CMD_END] = make_new_entry("end", "end command to end the script i guess", cmd_end, CMD_END, 0);
    syntax_arr[CMD_SET] = make_new_entry("set", "sets a value for internal variable, first arg for variable name and second for its value", cmd_set, CMD_SET, 2, STRING, STRING);
    syntax_arr[CMD_REMOVE] = make_new_entry("rem", "removes a variabl", cmd_remove, CMD_REMOVE, 1, STRING);
    syntax_arr[CMD_APPEND] = make_new_entry("append", "may concatenate a string to a variable", cmd_append, CMD_APPEND, 2, STRING, STRING);
    syntax_arr[CMD_MAKE_SDL_OBJECT] = make_new_entry("make_sdl_obj", "combines multiple numbers together, used later in sdl calls.\n\tsyntax: <var_name> <var_type(subarguments): SDL_Color(4), SDL_Point(2), SDL_Rect(4)>", cmd_make_sdl_object, CMD_MAKE_SDL_OBJECT, 1, STRING);

    syntax_arr[CMD_PRINT_VARS] = make_new_entry("print_env", "prints current variables and resources", cmd_print_env, CMD_PRINT_VARS, 0);

    syntax_arr[CMD_INIT_WINDOW] = make_new_entry("init_window", "filename, width and height", cmd_init_window, CMD_INIT_WINDOW, 3, STRING, INT, INT);
    syntax_arr[CMD_LOAD_IMAGE] = make_new_entry("load_image", "loads image from your disk", cmd_load_image, CMD_LOAD_IMAGE, 1, STRING);
    syntax_arr[CMD_CROP_IMAGE] = make_new_entry("crop_image", "crops image and creates new one - <name> <rect_name> <output_surface_name>", cmd_crop_image, CMD_CROP_IMAGE, 3, STRING, STRING, STRING);

    syntax_arr[CMD_SET_COLOR] = make_new_entry("set_color", "select a color from sdl_color object\n", cmd_set_color, CMD_SET_COLOR, 1, STRING);
    syntax_arr[CMD_RENDER_IMAGE] = make_new_entry("render_image", "copies an imeg to specified coordinates on ascreen\n\tname, src_rect, dest_rect", cmd_render_image, CMD_RENDER_IMAGE, 3, STRING, STRING, STRING);

    syntax_arr[CMD_RENDER_POINT] = make_new_entry("render_point", "rendrs point on certain sdl_point", cmd_render_point, CMD_RENDER_POINT, 1, STRING);
    syntax_arr[CMD_RENDER_LINE] = make_new_entry("render_line", "renders a line between 2 sdl_points", cmd_render_line, CMD_RENDER_LINE, 1, STRING);
    syntax_arr[CMD_RENDER_RECT] = make_new_entry("render_rect", "rneders rect", cmd_render_rect, CMD_RENDER_RECT, 1, STRING);

    char *out_filename = 0;

    put_entry(vars, "__internal__parse__more__", "yeah why not");

    int width,
        height;

    SDL_Window *window = 0;
    SDL_Renderer *renderer = 0;

    int current_line = 0;

    int initialized = 0;

    do
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

        if (syntax_arr[cmd_id].function(command, vars, resources) == FAIL)
        {
            fprintf(stderr, "error occured on line %d\n", current_line);
        }

        if (!initialized)
        {
            if (!eject_entry(vars, "__internal__window__initialized__"))
            {
                fprintf(stderr, "Window wasnt initialized, please specify window sizes with this command on your first line:\n\tinit_window <out_filename.png> <width> <height>\n");
                return -1;
            }
            else
            {
                renderer = str_to_ptr(get_entry(vars, "__internal__renderer__ptr__"));
                window = str_to_ptr(get_entry(vars, "__internal__window__ptr__"));
                width = atoi(get_entry(vars, "__iternal__window__width__"));
                height = atoi(get_entry(vars, "__iternal__window__height__"));
                out_filename = strdup(get_entry(vars, "__iternal__out__filename__"));
                initialized = 1;
            }
        }

        current_line++;
    } while (get_entry(vars, "__internal__parse__more__"));

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
        exit_all(window, renderer);

    // Free allocated memory and close file
    free_table(vars);
    free_table(resources);
    fclose(f);

    return 0;
}