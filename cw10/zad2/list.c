#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

list_t *new_list() {
    list_t *res = malloc(sizeof(list_t));
    res->root = NULL;
    return res;
}


void free_node(node_t *node) {
    if(node->next != NULL){
        free_node(node->next);
    }
    free(node->word);
    free(node);
}


void free_list(list_t *list){
    if(list->root != NULL){
        free_node(list->root);
    }
    free(list);
}

node_t *new_node(char *word){
    node_t *res = malloc(sizeof(node_t));
    res->word = malloc(MAX_WORD*sizeof(char));
    strcpy(res->word, word);
    res->count = 1;
    res->next = NULL;
    return res;
}

void insert_node(node_t *node, char *word){
    if(strcmp(node->word, word) == 0){
        node->count += 1;
    }
    else{
        if(node->next == NULL){
            node->next = new_node(word);
        }else{
            insert_node(node->next, word);
        }
    }
}

void insert_list(list_t *list, char *word){
    if(list->root == NULL){
        list->root = new_node(word);
    }
    else{
        insert_node(list->root, word);
    }
}



