#ifndef SCRIPT_CMD_ARGS_H
#define SCRIPT_CMD_ARGS_H

#include "hash_table.h"
#include "types.h"
#include <stdarg.h>

#define STRING 0
#define INT 1
#define FLOAT 2

typedef struct cmd_syntax
{
    char *name;
    char *args_expl;
    int internal_id;
    int min_args;
    int *argtypes;
} cmd_syntax;

int determine_type(char *str);
int is_syntax_good(splitted_words w, cmd_syntax syntax);
int eval_id_of_command(splitted_words w, cmd_syntax *syntax_arr, int total_commands);

cmd_syntax make_new_entry(char *name, char *expl, int id, int min_args, ...);

#endif