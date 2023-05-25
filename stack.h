#include <stdlib.h>
#include <stdio.h>

#ifndef STACK_H
#define STACK_H

typedef int stackitem;

typedef struct {
    int size;
    int maxsize;
    stackitem* start;
} stack;


stack* newstack();

stackitem pop(stack* s);

stackitem peek(stack* s);

void push(stack *s, stackitem si);

#endif
