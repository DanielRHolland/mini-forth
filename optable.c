#include "optable.h"
#include <stdio.h>
#include <string.h>

static void not(stack *s);
static void drop(stack *s);
static void over(stack *s);
static void rot(stack *s);
static void swap(stack *s);
static void eq(stack *s);
static void add(stack *s);
static void mult(stack *s);
static void s_div(stack *s);
static void sub(stack *s);
static void dup(stack *s);
static void popout(stack *s);
static void peekout(stack *s);

static void ifdirective(stack *s, int len, char* line, int* i);
static void defineopdirective(stack *s, int len, char* line, int* i);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-function-pointer-types"
// clang seems to not realise that the union can contain fp types beyond the first
static wordop optable[OPTABLE_MAX_SIZE] = {
    {".", builtin, {popout}},
    {"peek", builtin, {peekout}},
    {"+", builtin, {add}},
    {"-", builtin, {sub}},
    {"*", builtin, {mult}},
    {"/", builtin, {s_div}},
    {"dup", builtin, {dup}},
    {"not", builtin, {not}},
    {"=", builtin, {eq}},
    {"swap", builtin, {swap}},
    {"drop", builtin, {drop}},
    {"over", builtin, {over}},
    {"rot", builtin, {rot}},
    {"if", directive, {ifdirective}},
    {":", directive, {defineopdirective}},
};
static int optablelen = 15;
#pragma clang diagnostic pop

int defineop(int starti, char *input) {
    // name by which the function will be called
    char *opcode = malloc(sizeof(char) * WORD_LEN_LIMIT);
    int opcodei = 0;

    // code to be evaluated when function is called
    char *funcscript = malloc(sizeof(char) * DEFINED_FUNC_MAX_LENGTH);
    int funcscripti = 0;
    
    // skip ' ' and ':'
    while (input[starti] == ' ' || input[starti] == ':') {
        starti++;
    }

    // get name
    while (input[starti] != ' ' && opcodei < WORD_LEN_LIMIT) {
        opcode[opcodei++] = input[starti++];
    }
    opcode[opcodei] = '\0';

    // get code
    while (input[starti] != ';' && funcscripti < DEFINED_FUNC_MAX_LENGTH) {
        funcscript[funcscripti++] = input[starti++];
    }
    funcscript[funcscripti] = '\0';

    // optable bounds check 
    if (optablelen >= OPTABLE_MAX_SIZE) {
        // Error
        fprintf(stderr, "Error: optable reached max size, failed to create new user defined operation");
        exit(1);
    }
    // add op to end of table, and increment size
    optable[optablelen].word = opcode;
    optable[optablelen].optype = script;
    optable[optablelen].script = funcscript;
    optable[optablelen].scriptlen = funcscripti;
    optablelen++;
    return starti;
}

wordop* getop(char *word) {
    for (int i = 0; i < optablelen; i++) {
        if (!strcmp(optable[i].word, word)) {
            return &optable[i];
        }
    }
    return 0;
}

/* Implementations of builtin functions */

static void not(stack *s) {
    int x = pop(s);
    push(s, !x);
}

static void drop(stack *s) {
    pop(s);
}

static void over(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, y);
    push(s, x);
    push(s, y);
}

static void rot(stack *s) {
    int x = pop(s);
    int y = pop(s);
    int z = pop(s);
    push(s, y);
    push(s, x);
    push(s, z);
}

static void swap(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, x);
    push(s, y);
}

static void eq(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, x == y);
}

static void add(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, x + y);
}

static void mult(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, x * y);
}

static void s_div(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, y / x);
}

static void sub(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, y - x);
}

static void dup(stack *s) {
    int x = pop(s);
    push(s, x);
    push(s, x);
}

static void popout(stack *s) {
    printf("%d\n", pop(s));
}

static void peekout(stack *s) {
    int x = pop(s);
    push(s, x);
    printf("%d\n", x);
}


/* Directives */

static void ifdirective(stack *s, int len, char* line, int* i) {
    stackitem predicate = pop(s);
    if (!predicate) {
        while (len > *i && !(
                    line[*i-3] == 't' &&
                    line[*i-2] == 'h' &&
                    line[*i-1] == 'e' &&
                    line[*i] == 'n'
                    )) {
            (*i)++;
        }
    }
}

static void defineopdirective(stack *s, int len, char* line, int* i) {
    *i = defineop(*i, line);
}