#include <stdlib.h>
#include <stdbool.h>
#include "linked_lists.h"
#include "menu_system.h"

struct Node *create_linked_list(int data)
{
    struct Node *node = malloc(sizeof(struct Node));
    if (node == NULL)
    {
        report_alloc_error(sizeof(struct Node));
    }

    node->data = data;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

struct Node *get_last_node(struct Node *head)
{
    while (head->next != NULL)
    {
        head = head->next;
    }
    return head;
}

// Finds the location of a node given the data
// Returns the index of the node if it exists; -1 otherwise
int find_node_location(struct Node *head, int data)
{
    struct Node *curr = head;

    int location = -1;
    while (curr != NULL)
    {
        location++;
        if (curr->data == data)
        {
            return location;
        }
        curr = curr->next;
    }

    return -1;
}

bool does_node_exist(struct Node *head, int data)
{
    int node_location = find_node_location(head, data);
    if (node_location != -1)
    {
        return true;
    }
    return false;
}

// Returns the size of the linked list
int get_size(struct Node *head)
{
    int size = 0;
    struct Node *curr = head;
    while (curr != NULL)
    {
        size++;
        curr = curr->next;
    }
    return size;
}

// Deletes the first node with matching data
// Returns 1 if the node was successfully deleted; 0 otherwise
int delete_node_by_data(struct Node **head, int data)
{
    struct Node *curr = *head;
    while (curr != NULL)
    {
        if (curr->data == data) // If the node's data matches the searched data
        {
            if (curr->next != NULL) // Updates the next node's previous (if it exists) node to be the current node's previous
            {
                curr->next->prev = curr->prev;
            }
            if (curr->prev != NULL) // Updates the previous node's next (if it exists) to be the current node's next
            {
                curr->prev->next = curr->next;
            }
            else // If the previous node doesn't exist, then makes the passed in head point to the next node
            {
                *head = curr->next;
            }
            free(curr);
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

void insert_after_last_node(struct Node **head, int data)
{
    struct Node *new_last_node = create_linked_list(data);
    if (*head == NULL)
    {
        *head = new_last_node;
    }
    else
    {
        struct Node *curr_last_node = get_last_node(*head);
        curr_last_node->next = new_last_node;
        new_last_node->prev = curr_last_node;
    }
}

void free_linked_list(struct Node **head)
{
    struct Node *curr = *head;
    while (curr != NULL)
    {
        struct Node *next = curr->next;
        free(curr);
        curr = next;
    }
    *head = NULL;
}