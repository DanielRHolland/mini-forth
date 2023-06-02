#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

stack* stack_new() {
    stack* s = (stack*)malloc(sizeof(stack));
    s->size = 0;
    s->maxsize = 1024;
    s->start = (stackitem*)malloc(sizeof(stackitem) * s->maxsize);
    return s;
}

void exitunderflow() {
    fprintf(stderr, "Error Stack Underflow");
    exit(1);
}

stackitem stack_pop(stack* s) {
    if (s->size > 0) {
        s->size = s->size - 1;
        stackitem si = s->start[s->size];
        return si;
    } else {
        // tried to pop empty stack
        exitunderflow();
        return 0;
    }
}

stackitem stack_peek(stack* s) {
    if (s->size > 0) {
        return s->start[s->size - 1];
    } else {
        // tried to pop empty stack
        exitunderflow();
        return 0;
    }
}

void stack_push(stack *s, stackitem si) {
//        fprintf(stderr, "pushing %d", si);
    if (s->size >= s->maxsize) {
        fprintf(stderr, "Error: Stack Overflow");
        exit(1);
    } else {
        s->start[s->size] = si;
        s->size = s->size + 1;
    }
}

int stack_depth(stack* s) {
    return s->size;
}

/**
 * Appends string representing whole stack to the string buffer passed in
 */
void stack_tostringappend(stack* s, int sbmaxlen, char* sb) {
    int i = strlen(sb);
    if (i >= sbmaxlen) {
        fprintf(stderr, "Error: Output would overflow buffer if printed");
        exit(0); 
    }
    sprintf(&(sb[i]), "<%d>", s->size);
    i = strlen(sb);
    if (i >= sbmaxlen) {
        fprintf(stderr, "Error: Output would overflow Buffer if printed");
        exit(0); 
    }
    for (int j = 0; j < s->size; j++) {
        sprintf(&(sb[i]), " %d", s->start[j]);
        i = strlen(sb);
        if (i >= sbmaxlen) {
            fprintf(stderr, "Error: Output would overflow Buffer if printed");
            exit(0); 
        }
    }
    sprintf(&(sb[i]), "\n");
}

void stack_roll(stack* s) {
    int posfromtop = stack_pop(s); // top is 0, bottom is s->size - 1
    int maxindex = s->size - 1;
    int i = maxindex - posfromtop;
    if (i >= 0) {
        int newtop = s->start[i];
        for (; i + 1 < s->size; i++) {
            s->start[i] = s->start[i+1];
        }
        s->start[i] = newtop;
    } else {
        // tried to pop empty stack
        exitunderflow();
    }
}

void stack_pick(stack *s) {
    int posfromtop = stack_pop(s); // top is 0, bottom is s->size - 1
    int maxindex = s->size - 1;
    if (s->size > posfromtop) {
        stack_push(s, s->start[maxindex - posfromtop]);
    } else {
        // tried to pop empty stack
        exitunderflow();
    }
}

void stack_clear(stack* s) {
    s->size = 0;
}
