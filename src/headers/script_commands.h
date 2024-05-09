#ifndef SCRIPT_COMMANDS_H
#define SCRIPT_COMMANDS_H

#include "script_cmd_args.h"

int cmd_empty(splitted_words command, hash_table **vars, hash_table **resources);
int cmd_end(splitted_words command, hash_table **vars, hash_table **resources);

int cmd_set(splitted_words command, hash_table **vars, hash_table **resources);
int cmd_remove(splitted_words command, hash_table **vars, hash_table **resources);
int cmd_append(splitted_words command, hash_table **vars, hash_table **resources);

int cmd_print_env(splitted_words command, hash_table **vars, hash_table **resources);

int cmd_init_window(splitted_words command, hash_table **vars, hash_table **resources);
int cmd_make_sdl_object(splitted_words command, hash_table **vars, hash_table **resources);

int cmd_set_color(splitted_words command, hash_table **vars, hash_table **resources);
int cmd_load_image(splitted_words command, hash_table **vars, hash_table **resources);

int cmd_render_image(splitted_words command, hash_table **vars, hash_table **resources);
int cmd_render_point(splitted_words command, hash_table **vars, hash_table **resources);
int cmd_render_line(splitted_words command, hash_table **vars, hash_table **resources);
int cmd_render_rect(splitted_words command, hash_table **vars, hash_table **resources);

#endif