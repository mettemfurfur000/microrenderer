#include "headers/script_cmd_args.h"

int determine_type(char *str)
{
    int numbers = 0;
    int characters = 0;
    int dots = 0;
    int symbols = 0;
    int spaces = 0;
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
        if (isspace(c))
            spaces++;
    }

    if (characters > 0 || symbols > 0 || spaces > 0)
        return STRING;
    if (dots == 0 && numbers > 0)
        return INT;
    if (dots == 1 && numbers > 0)
        return FLOAT;
    return STRING;
}

int is_syntax_good(splitted_words w, cmd_syntax syntax)
{
    if (w.len < syntax.min_args + 1)
    {
        fprintf(stderr, "Syntax error: not enough arguments, %s requires at least %d\n", syntax.name, syntax.min_args);
        fprintf(stderr, "Command syntax details: %s\n", syntax.args_expl);
        return FAIL;
    }

    for (int i = 0; i < syntax.min_args; i++)
    {
        int req_type = syntax.argtypes[i];

        if (req_type != STRING && determine_type(w.words[i + 1]) != req_type)
        {
            fprintf(stderr, "Syntax error: wrong type, argument #%d must be %s\n", i + 1,
                    req_type ? req_type == 1 ? "\"int\"" : "\"float\"" : "\"string\"");
            fprintf(stderr, "Command syntax details: %s\n", syntax.args_expl);
            return FAIL;
        }
    }

    return SUCCESS;
}

int eval_id_of_command(splitted_words w, cmd_syntax *syntax_arr, int total_commands)
{
    if (w.len == 0)
        return 0;
    for (int i = 0; i < total_commands; i++)
        if (strcmp(w.words[0], syntax_arr[i].name) == 0)
            return syntax_arr[i].internal_id;

    return -1;
}

cmd_syntax make_new_entry(char *name, char *expl, int id, int min_args, ...)
{
    va_list valist;
    cmd_syntax cs;
    va_start(valist, min_args);

    cs.internal_id = id;
    cs.name = name;
    cs.args_expl = expl;
    cs.min_args = min_args;
    cs.argtypes = calloc(min_args, sizeof(int));

    for (int i = 0; i < min_args; i++)
        cs.argtypes[i] = va_arg(valist, int);

    va_end(valist);
    return cs;
}
