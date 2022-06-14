/**
 *  list.h
 *
 *  Full Name: Jiayue Zhang
 *  Course section: LE/EECS3221 M - Operating System Fundamentals (Winter 2021-2022)
 *  Description of the program: list data structure containing the tasks in the system
 *  
 */
 
#include "process.h"

struct node {
    Process *process;
    struct node *next;
    struct node *last;
};

// insert and delete operations.
void insert(struct node **head, Process *process);
void delete(struct node **head, Process *process);
void traverse(struct node *head);
int getPID(struct node *head);
