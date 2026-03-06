#ifndef LINKED_LISTS_H_INCLUDED
#define LINKED_LISTS_H_INCLUDED
struct Node
{
    int data;
    struct Node *next;
    struct Node *prev;
};

struct Node *create_linked_list(int data);
int find_node_location(struct Node *head, int data);
bool does_node_exist(struct Node *head, int data);
int get_size(struct Node *head);
int delete_node_by_data(struct Node **head, int data);
struct Node *get_last_node(struct Node *head);
void insert_after_last_node(struct Node **head, int data);
void free_linked_list(struct Node **head);
#endif