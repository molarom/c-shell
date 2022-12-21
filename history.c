#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

Node *create_node(char *command) {
    Node *node = malloc(sizeof(Node));
    node->command = strdup(command);
    node->prev = NULL;
    node->next = NULL;
    return node;
}

List *create_list() {
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void add_command(List *list, char *command) {
    Node *node = create_node(command);
    if (list->size == 0) {
        list->head = node;
        list->tail = node;
    } else {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

void remove_command(List *list, int index)
{
    if (index < 0 || index >= list->size) {
        fprintf(stderr, "Error: index out of range\n");
        return;
    }
    Node *node = list->head;
    for (int i = 0; i < index; i++) {
        node = node->next;
    }
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    if (node == list->head) {
        list->head = node->next;
    }
    if (node == list->tail) {
        list->tail = node->prev;
    }
    free(node->command);
    free(node);
    list->size--;
}

void print_history(List *list)
{
    Node *node = list->head;
    int i = 0;
    while (node->next != NULL) {
        printf("%d: %s", i, node->command);
        node = node->next;
        i++;
    }
}

void free_list(List *list) {
    Node *node = list->head;
    while (node != NULL) {
        Node *next = node->next;
        free(node->command);
        free(node);
        node = next;
    }
    free(list);
}
