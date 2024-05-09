#include "headers/script_commands.h"
#include "headers/types.h"
#include "headers/render.h"
#include "headers/misc.h"

enum SDL_OBJECT_TYPES
{
    UNKNOWN = -1,
    SDL_COLOR,
    SDL_POINT,
    SDL_RECT
};

int det_sdl_type(char *word)
{
    char buffer[256] = {};
    strcpy(buffer, word);
    for (int i = 0; i < sizeof(buffer); i++)
        buffer[i] = tolower(buffer[i]);

    if (!strcmp(buffer, "sdl_color"))
        return SDL_COLOR;
    if (!strcmp(buffer, "sdl_point"))
        return SDL_POINT;
    if (!strcmp(buffer, "sdl_rect"))
        return SDL_RECT;
    return UNKNOWN;
}

#define reverse_value_filler_bi(var, command, startswith, field_a, field_b) \
    switch (command.len)                                                    \
    {                                                                       \
    case startswith + 2:                                                    \
        (var).field_b = atoi(command.words[startswith + 1]);                \
    case startswith + 1:                                                    \
        (var).field_a = atoi(command.words[startswith]);                    \
    }

#define reverse_value_filler_quad(var, command, startswith, field_a, field_b, field_c, field_d) \
    switch (command.len)                                                                        \
    {                                                                                           \
    case startswith + 4:                                                                        \
        (var).field_d = atoi(command.words[startswith + 3]);                                    \
    case startswith + 3:                                                                        \
        (var).field_c = atoi(command.words[startswith + 2]);                                    \
    case startswith + 2:                                                                        \
        (var).field_b = atoi(command.words[startswith + 1]);                                    \
    case startswith + 1:                                                                        \
        (var).field_a = atoi(command.words[startswith]);                                        \
    }

#define make_heap_object(type, name) type *name = malloc(sizeof(type))

// basicly word[0] is aways a command name and should be ignored

int cmd_empty(splitted_words command, hash_table **vars, hash_table **resources)
{
    return SUCCESS;
}

int cmd_end(splitted_words command, hash_table **vars, hash_table **resources)
{
    remove_entry(vars, "__internal__parse__more__");
    return SUCCESS;
}

int cmd_set(splitted_words command, hash_table **vars, hash_table **resources)
{
    put_entry(vars, command.words[1], command.words[2]);
    return SUCCESS;
}

int cmd_remove(splitted_words command, hash_table **vars, hash_table **resources)
{
    char *value = get_entry(vars, command.words[1]);
    if (determine_type(value) == POINTER)
        free(str_to_ptr(value));
    remove_entry(vars, command.words[1]);
    return SUCCESS;
}

int cmd_append(splitted_words command, hash_table **vars, hash_table **resources)
{
    char *value = get_entry(vars, command.words[1]);
    if (!value)
    {
        fprintf(stderr, "Entry '%s' does not exist, creating value instead\n", command.words[1]);
        cmd_set(command, vars, resources);
        return SUCCESS;
    }

    char *new_value = (char *)malloc(strlen(value) + strlen(command.words[2]) + 1);

    if (!new_value)
    {
        perror("Error allocating memory for new_value");
        return FAIL;
    }

    strcpy(new_value, value);
    strcat(new_value, command.words[2]);
    put_entry(vars, command.words[1], new_value);
    free(new_value);

    return SUCCESS;
}

int cmd_make_sdl_object(splitted_words command, hash_table **vars, hash_table **resources)
{
    int width = atoi(get_entry(vars, "__iternal__window__width__"));
    int height = atoi(get_entry(vars, "__iternal__window__height__"));

    //   cmd_make_sdl_object type name ? ? ? ?
    // i:0                   1    2    3 4 5 6

    switch (det_sdl_type(command.words[1]))
    {
    case UNKNOWN:
        fprintf(stderr, "cmd_make_sdl_object error: unknown type of object, abort\n");
        return FAIL;
    case SDL_COLOR:
        // command is make_sdl_object sdl_color ? ? ? ?
        make_heap_object(SDL_Color, color);
        // default values
        color->r = 255;
        color->g = 255;
        color->b = 255;
        color->a = 255;

        reverse_value_filler_quad(*color, command, 3, r, g, b, a);

        put_entry(vars, command.words[2], ptr_to_str(color));
        break;
    case SDL_POINT:
        // make_sdl_object sdl_point ? ?
        make_heap_object(SDL_Point, point);
        point->x = rand() % width;
        point->y = rand() % height;

        reverse_value_filler_bi(*point, command, 3, x, y);

        put_entry(vars, command.words[2], ptr_to_str(point));
        break;
    case SDL_RECT:
        // command is make_sdl_object sdl_rect ? ? ? ?
        make_heap_object(SDL_Rect, rect);

        rect->w = rand() % width / 4;
        rect->h = rand() % height / 4;

        rect->x = rand() % (width - rect->w);
        rect->y = rand() % (height - rect->h);

        reverse_value_filler_quad(*rect, command, 3, x, y, h, w);

        put_entry(vars, command.words[2], ptr_to_str(rect));
        break;
    }

    return SUCCESS;
}

int cmd_print_env(splitted_words command, hash_table **vars, hash_table **resources)
{
    printf("variables table:\n");
    print_table(vars);

    printf("resources table:\n");
    print_table(resources);
    return SUCCESS;
}

int cmd_init_window(splitted_words command, hash_table **vars, hash_table **resources)
{
    put_entry(vars, "__iternal__out__filename__", command.words[1]);

    put_entry(vars, "__iternal__window__width__", command.words[2]);
    put_entry(vars, "__iternal__window__height__", command.words[3]);

    void *renderer, *window;

    init_all(atoi(command.words[2]), atoi(command.words[3]), (SDL_Window **)&window, (SDL_Renderer **)&renderer);
    SDL_RenderClear(renderer);

    put_entry(vars, "__internal__renderer__ptr__", ptr_to_str(renderer));
    put_entry(vars, "__internal__window__ptr__", ptr_to_str(window));

    put_entry(vars, "__internal__window__initialized__", "yeah");
    put_entry(vars, "__internal__parse__more__", "yeah");

    return SUCCESS;
}

int cmd_set_color(splitted_words command, hash_table **vars, hash_table **resources)
{
    char *color_var = get_entry(vars, command.words[1]);
    SDL_Color c = {255, 255, 255, 255};
    if (color_var)
        c = *(SDL_Color *)str_to_ptr(color_var);
    else
    {
        reverse_value_filler_quad(c, command, 1, r, g, b, a)
    }
    SDL_SetRenderDrawColor(str_to_ptr(get_entry(vars, "__internal__renderer__ptr__")), c.r, c.g, c.b, c.a);

    return SUCCESS;
}

int cmd_load_image(splitted_words command, hash_table **vars, hash_table **resources)
{
    void *surf = IMG_Load(command.words[1]);
    if (!surf)
    {
        fprintf(stderr, "cmd_load_image error: no such file %s\n", command.words[1]);
        return FAIL;
    }
    put_entry(resources, command.words[1], ptr_to_str(surf));
    return SUCCESS;
}

int cmd_render_image(splitted_words command, hash_table **vars, hash_table **resources)
{
    SDL_Renderer *r = str_to_ptr(get_entry(vars, "__internal__renderer__ptr__"));
    SDL_Surface *s = str_to_ptr(get_entry(resources, command.words[1]));
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);

    SDL_Rect *src_ptr = (SDL_Rect *)str_to_ptr(get_entry(vars, command.words[2]));
    SDL_Rect *dest_ptr = (SDL_Rect *)str_to_ptr(get_entry(vars, command.words[3]));
    SDL_RenderCopy(r, t, src_ptr, dest_ptr);

    return SUCCESS;
}

int cmd_render_point(splitted_words command, hash_table **vars, hash_table **resources)
{
    SDL_Point p = *(SDL_Point *)str_to_ptr(get_entry(vars, command.words[1]));
    SDL_RenderDrawPoint(str_to_ptr(get_entry(vars, "__internal__renderer__ptr__")),
                        p.x, p.y);
    return SUCCESS;
}

int cmd_render_line(splitted_words command, hash_table **vars, hash_table **resources)
{
    char *point_a_ptr = get_entry(vars, command.words[1]);
    char *point_b_ptr = get_entry(vars, command.words[2]);

    SDL_Point a = {255, 255};
    SDL_Point b = {255, 255};

    if (!point_a_ptr && !point_b_ptr)
    {
        a.x = atoi(command.words[1]);
        a.y = atoi(command.words[2]);
        b.x = atoi(command.words[3]);
        b.y = atoi(command.words[4]);
    }
    else
    {
        a = *(SDL_Point *)str_to_ptr(point_a_ptr);
        b = *(SDL_Point *)str_to_ptr(point_b_ptr);
    }

    SDL_RenderDrawLine(str_to_ptr(get_entry(vars, "__internal__renderer__ptr__")), a.x, a.y, b.x, b.y);
    return SUCCESS;
}

int cmd_render_rect(splitted_words command, hash_table **vars, hash_table **resources)
{
    char *rect_ptr = get_entry(vars, command.words[1]);

    SDL_Rect rect;

    if (!rect_ptr)
        reverse_value_filler_quad(rect, command, 1, x, y, w, h) else rect = *(SDL_Rect *)str_to_ptr(rect_ptr);

    SDL_RenderDrawRect(str_to_ptr(get_entry(vars, "__internal__renderer__ptr__")), &rect);
    return SUCCESS;
}