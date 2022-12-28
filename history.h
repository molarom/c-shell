#ifndef HISTORY_H_
#define HISTORY_H_

typedef struct node {
    char *command;
    struct node *prev;
    struct node *next;
} Node;

// Keep track of the ends of our list.
typedef struct list {
    Node *head;
    Node *tail;
    int size;
} List;

// Creates a new node with the given command
static Node *create_node(char *command);

// Initializes an empty list
List *create_list();

// Adds a new command to the end of the list
void add_command(List *list, char *command);

// Removes the command at the given index from the list
void remove_command(List *list, int index);

// Print out contents.
void print_history(List *list);

void free_list(List *list); 

#endif
