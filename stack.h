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


stack* stack_new();

stackitem stack_pop(stack* s);

stackitem stack_peek(stack* s);

void stack_push(stack *s, stackitem si);

int stack_depth(stack *s);

void stack_printall(stack *s);

void stack_roll(stack *s);

void stack_pick(stack *s);

#endif
