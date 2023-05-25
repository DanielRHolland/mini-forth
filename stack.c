#include "stack.h"

stack* newstack() {
    stack* s = (stack*)malloc(sizeof(stack));
    s->size = 0;
    s->maxsize = 1024;
    s->start = (stackitem*)malloc(sizeof(stackitem) * s->maxsize);
    return s;
}

stackitem pop(stack* s) {
    if (s->size > 0) {
        s->size = s->size - 1;
        stackitem si = s->start[s->size];
        return si;
    } else {
        // tried to pop empty stack
        return 0;
    }
}

stackitem peek(stack* s) {
    if (s->size > 0) {
        return s->start[s->size - 1];
    } else {
        // tried to pop empty stack
        return 0;
    }
}

void push(stack *s, stackitem si) {
//        fprintf(stderr, "pushing %d", si);
    if (s->size >= s->maxsize) {
        fprintf(stderr, "Error Stack Overflow");
        exit(1);
    } else {
        s->start[s->size] = si;
        s->size = s->size + 1;
    }
}
