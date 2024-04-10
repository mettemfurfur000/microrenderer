#include "hash_table.c"
#include "types.h"

enum ARG_TYPE
{
    STRING = 0,
    INT,
    FLOAT
} ARG_TYPE;

typedef struct cmd_syntax
{
    char *name;
    int min_args;
    int *argtypes;
} cmd_syntax;

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

int determine_type(char *str)
{
    int numbers = 0;
    int characters = 0;
    int dots = 0;
    int symbols = 0;
    int len = strlen(str);

    for (int i = 0; i < len; i++)
    {
        char c = str[i];
        if (isalpha(c))
            characters++;
        if (isdigit(c))
            numbers++;
        if (c == '.')
            dots++;
        if (ispunct(c))
            symbols++;
    }

    if (characters > 0 || symbols > 0)
        return STRING;
    if (dots == 0 && numbers > 0)
        return INT;
    if (dots == 1 && numbers > 0)
        return FLOAT;
    return STRING;
}

int check_command_syntax(splitted_words w, cmd_syntax syntax)
{
    if (w.len < syntax.min_args + 1)
    {
        fprintf(stderr, "Syntax error: not enough arguments, %s requires at least %d\n", syntax.name, syntax.min_args);
        return FAIL;
    }

    for (int i = 0; i < syntax.min_args; i++)
    {
        int req_type = syntax.argtypes[i];
        if (determine_type(w.words[i + 1]) != req_type)
        {
            fprintf(stderr, "Syntax error: wrong type, argument #%d must be %s\n", i + 1,
                    req_type ? req_type == 1 ? "\"int\"" : "\"float\"" : "\"string\"");
            return FAIL;
        }
    }

    return SUCCESS;
}

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

// int test()
// { you can just allocate things on a stack maybe huh...
//     cmd_syntax syntaxes[] = {
//         {"test_cmd", 2, {1, 1}, 2}};
// }