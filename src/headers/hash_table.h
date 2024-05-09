#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "types.h"
#define TABLE_SIZE 113

typedef struct hash_table
{
    char *key;
    char *value;
    struct hash_table *next;
} hash_table;

hash_table **alloc_table();
void free_table(hash_table **table);

void put_entry(hash_table **table, char *key, char *value);
char *get_entry(hash_table **table, char *key);
int remove_entry(hash_table **table, char *key);
char *eject_entry(hash_table **table, char *key);

void print_table(hash_table **table);

int actual_size_of_table(hash_table **table);

#endif