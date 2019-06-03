#pragma once

#define MAX_WORD 50

typedef struct node_t {
    struct node_t *next;
    char *word;
    int count;
} node_t;

typedef struct list_t {
    node_t *root;
} list_t;



list_t *new_list();


void free_node(node_t *node);

void free_list(list_t *list);

node_t *new_node(char *word);

void insert_node(node_t *node, char *word);

void insert_list(list_t *list, char *word);