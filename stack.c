#include "stack.h"
#include "errorhandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

stack* stack_new(errorhandler errorhandler) {
    stack* s = (stack*)malloc(sizeof(stack));
    s->size = 0;
    s->maxsize = 1024;
    s->start = (stackitem*)malloc(sizeof(stackitem) * s->maxsize);
    s->errorhandler = errorhandler;
    return s;
}

void stack_free(stack* s) {
    free(s->start);
    free(s);
}


#define ERROR_STACK_EMPTY_CANNOT_POP "Error: Cannot pop from an empty stack"
#define ERROR_STACK_OVERFLOW "Error: Stack Overflow"
#define ERROR_OUTPUT_BUFFER_OVERFLOW "Error: Output would overflow buffer if printed"

stackitem stack_pop(stack* s) {
    if (s->size > 0) {
        s->size = s->size - 1;
        stackitem si = s->start[s->size];
        return si;
    } else {
        // tried to pop empty stack
        s->errorhandler(ERROR_STACK_EMPTY_CANNOT_POP);
        return 0;
    }
}

stackitem stack_peek(stack* s) {
    if (s->size > 0) {
        return s->start[s->size - 1];
    } else {
        // tried to pop empty stack
        s->errorhandler(ERROR_STACK_EMPTY_CANNOT_POP);
        return 0;
    }
}

void stack_push(stack *s, stackitem si) {
//        fprintf(stderr, "pushing %d", si);
    if (s->size >= s->maxsize) {
        s->errorhandler(ERROR_STACK_OVERFLOW);
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
        s->errorhandler(ERROR_OUTPUT_BUFFER_OVERFLOW);
    }
    sprintf(&(sb[i]), "<%d>", s->size);
    i = strlen(sb);
    if (i >= sbmaxlen) {
        s->errorhandler(ERROR_OUTPUT_BUFFER_OVERFLOW);
    }
    for (int j = 0; j < s->size; j++) {
        sprintf(&(sb[i]), " %d", s->start[j]);
        i = strlen(sb);
        if (i >= sbmaxlen) {
            s->errorhandler(ERROR_OUTPUT_BUFFER_OVERFLOW);
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
        s->errorhandler(ERROR_STACK_EMPTY_CANNOT_POP);
    }
}

void stack_pick(stack *s) {
    int posfromtop = stack_pop(s); // top is 0, bottom is s->size - 1
    int maxindex = s->size - 1;
    if (s->size > posfromtop) {
        stack_push(s, s->start[maxindex - posfromtop]);
    } else {
        // tried to pop empty stack
        s->errorhandler(ERROR_STACK_EMPTY_CANNOT_POP);
    }
}

void stack_clear(stack* s) {
    s->size = 0;
}
