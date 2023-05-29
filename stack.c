#include "stack.h"

stack* stack_new() {
    stack* s = (stack*)malloc(sizeof(stack));
    s->size = 0;
    s->maxsize = 1024;
    s->start = (stackitem*)malloc(sizeof(stackitem) * s->maxsize);
    return s;
}

stackitem stack_pop(stack* s) {
    if (s->size > 0) {
        s->size = s->size - 1;
        stackitem si = s->start[s->size];
        return si;
    } else {
        // tried to pop empty stack
        return 0;
    }
}

stackitem stack_peek(stack* s) {
    if (s->size > 0) {
        return s->start[s->size - 1];
    } else {
        // tried to pop empty stack
        return 0;
    }
}

void stack_push(stack *s, stackitem si) {
//        fprintf(stderr, "pushing %d", si);
    if (s->size >= s->maxsize) {
        fprintf(stderr, "Error Stack Overflow");
        exit(1);
    } else {
        s->start[s->size] = si;
        s->size = s->size + 1;
    }
}

int stack_depth(stack *s) {
    return s->size;
}

void stack_printall(stack *s) {
    printf("<%d>", s->size);
    for (int i = 0; i < s->size; i++) {
        printf(" %d", s->start[i]);
    }
    printf("\n");
}

void stack_roll(stack *s) {
    int posfromtop = stack_pop(s); // top is 0, bottom is s->size - 1
    const int maxindex = s->size - 1;
    int i = maxindex - posfromtop;
    if (i >= 0) {
        int newtop = s->start[i];
        for (; i + 1 < s->size; i++) {
            s->start[i] = s->start[i+1];
        }
        s->start[i] = newtop;
    }
}

void stack_pick(stack *s) {
    int posfromtop = stack_pop(s); // top is 0, bottom is s->size - 1
    const int maxindex = s->size - 1;
    if (s->size > posfromtop) {
        stack_push(s, s->start[maxindex - posfromtop]);
    } else {
        // no item found
        stack_push(s, 0);
    }
}
