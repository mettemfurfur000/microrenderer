#include "headers/hash_table.h"

unsigned long hash_function(char *key)
{
	// thanks https://t.me/codemaniacbot
	unsigned long hash = 5381;
	int c;
	while ((c = *key++))
	{
		hash = ((hash << 5) + hash) + c;
	}
	return hash % TABLE_SIZE;
}

hash_table *alloc_node()
{
	return (hash_table *)calloc(sizeof(hash_table), 1);
}

void free_node(hash_table *node)
{
	free(node->key);
	free(node->value);
	free(node);
}

void copy_key(hash_table *node, char *key)
{
	if (node->key)
		free(node->key);

	node->key = (char *)calloc(strlen(key) + 1, 1);
	strcpy(node->key, key);
}

void copy_value(hash_table *node, char *value)
{
	if (node->value)
		free(node->value);

	node->value = (char *)calloc(strlen(value) + 1, 1);
	strcpy(node->value, value);
}

void copy_all(hash_table *node, char *key, char *value)
{
	if (node->key)
		free(node->key);

	node->key = (char *)calloc(strlen(key) + 1, 1);
	strcpy(node->key, key);

	if (node->value)
		free(node->value);

	node->value = (char *)calloc(strlen(value) + 1, 1);
	strcpy(node->value, value);
}

hash_table **alloc_table()
{
	return (hash_table **)calloc(TABLE_SIZE, sizeof(hash_table *));
}

void free_table(hash_table **table)
{
	// thanks https://t.me/codemaniacbot
	hash_table *node;
	hash_table *next_node;
	for (int i = 0; i < TABLE_SIZE; ++i)
	{
		node = table[i];
		while (node != NULL)
		{
			next_node = node->next;
			free_node(node);
			node = next_node;
		}
	}
}

void put_entry(hash_table **table, char *key, char *value)
{
	// thanks https://t.me/codemaniacbot
	unsigned long hash = hash_function(key);
	hash_table *node = table[hash];
	if (node == NULL)
	{
		node = alloc_node();
		copy_all(node, key, value);
		table[hash] = node;
		return;
	}
	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
		{
			copy_value(node, value);
			return;
		}
		if (node->next == NULL)
		{
			node->next = alloc_node();
			copy_all(node->next, key, value);
			return;
		}
		node = node->next;
	}
}

char *get_entry(hash_table **table, char *key)
{
	// thanks https://t.me/codemaniacbot
	unsigned long hash = hash_function(key);
	hash_table *node = table[hash];

	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
		{
			return node->value;
		}

		node = node->next;
	}
	return NULL;
}

void print_table(hash_table **table)
{
	hash_table *node;
	hash_table *next_node;

	for (int i = 0; i < TABLE_SIZE; ++i)
	{
		node = table[i];
		while (node != NULL)
		{
			next_node = node->next;
			printf("    %s %s %s\n", node->key, strlen(node->value) ? "=" : "", node->value);
			node = next_node;
		}
	}
}

int remove_entry(hash_table **table, char *key)
{
	unsigned long hash = hash_function(key);
	hash_table *prev = NULL;
	hash_table *node = table[hash];

	while (node != NULL && strcmp(node->key, key) != 0)
	{
		prev = node;
		node = node->next;
	}

	if (node == NULL)
		return FAIL;

	if (prev == NULL)
		table[hash] = node->next;
	else
		prev->next = node->next;

	free_node(node);

	return SUCCESS;
}

char *eject_entry(hash_table **table, char *key)
{
	static char *internal_buffer = 0;

	char *value = get_entry(table, key);
	if (!value)
		return FAIL;

	if (internal_buffer)
		free(internal_buffer);
	internal_buffer = malloc(strlen(value) + 1);

	strcpy(internal_buffer, value);
	remove_entry(table, key);

	return internal_buffer;
}

int actual_size_of_table(hash_table **table)
{
	int size = 0;
	hash_table *node;

	for (int i = 0; i < TABLE_SIZE; ++i) // goes wide
	{
		node = table[i];

		while (node != NULL)
		{
			size += 1;

			node = node->next;
		}
	}
	return size;
}