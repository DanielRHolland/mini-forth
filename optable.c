#include "optable.h"
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

static wordop optable[OPTABLE_MAX_SIZE] = {
    {".", false, popout},
    {"peek", false, peekout},
    {"+", false, add},
    {"-", false, sub},
    {"*", false, mult},
    {"/", false, s_div},
    {"dup", false, dup},
    {"not", false, not},
    {"=", false, eq},
    {"swap", false, swap},
    {"drop", false, drop},
    {"over", false, over},
    {"rot", false, rot},
};
static int optablelen = 13;

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
    optable[optablelen].isscript = true;
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
