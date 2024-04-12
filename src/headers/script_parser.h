#ifndef SCRIPT_PARSER_H
#define SCRIPT_PARSER_H

#include "render.h"
#include "hash_table.h"
#include "misc.h"
#include "script_cmd_args.h"

int read_config_line(FILE *f, splitted_words *spl_w);
int parse_config(char *filename);

#endif