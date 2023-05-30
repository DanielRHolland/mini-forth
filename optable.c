#include "optable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
static void mod(stack *s);
static void negate(stack *s);
static void s_abs(stack *s);
static void max(stack *s);
static void min(stack *s);
static void sub(stack *s);
static void dup(stack *s);
static void popout(stack *s);
static void peekout(stack *s);
static void donothing(stack *s);
static void depth(stack *s);

static void ifdirective(stack *s, int len, char* line, int* i, optable* ot);
static void defineop(stack *s, int len, char* line, int* i, optable* ot);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-function-pointer-types"
// clang seems to not realise that the union can contain fp types beyond the first
const static wordop inittable[] = {
    {".", builtin, {popout}},
    {"peek", builtin, {peekout}},
    {"+", builtin, {add}},
    {"-", builtin, {sub}},
    {"*", builtin, {mult}},
    {"/", builtin, {s_div}},
    {"negate", builtin, {negate}},
    {"abs", builtin, {s_abs}},
    {"mod", builtin, {mod}},
    {"max", builtin, {max}},
    {"min", builtin, {min}},
    {"dup", builtin, {dup}},
    {"not", builtin, {not}},
    {"=", builtin, {eq}},
    {"swap", builtin, {swap}},
    {"drop", builtin, {drop}},
    {"over", builtin, {over}},
    {"rot", builtin, {rot}},
    {"pick", builtin, {stack_pick}},
    {"roll", builtin, {stack_roll}},
    {"then", builtin, {donothing}},
    {"depth", builtin, {depth}},
    {".s", builtin, {stack_printall}},
    {"clearstack", builtin, {stack_clear}},
    {"if", directive, {ifdirective}},
    {":", directive, {defineop}},
};

//static int optable->len = 26;
#pragma clang diagnostic pop

compileditem* optable_compilewords(optable* ot, int len, char** script) {
    compileditem* oplist = malloc(sizeof(compileditem) * len);
    for (int i = 0; i < len; i++) {
        wordop* wordop = optable_getop(ot, script[i]);
        if (wordop) {
            oplist[i].isliteral = false;
            oplist[i].wordop = wordop;
        } else {
            oplist[i].isliteral = true;
            oplist[i].literal = atoi(script[i]);
        }
    }
    return oplist;
}

void optable_addop(optable* ot, char* name, int len, char** words) {
    ot->optable[ot->len].word = malloc(sizeof(name));
    strcpy(ot->optable[ot->len].word, name);
    ot->optable[ot->len].optype = compiled;
    ot->optable[ot->len].oplistlen = len;
    compileditem* oplist = optable_compilewords(ot, len, words);
    ot->optable[ot->len].oplist = oplist;
    ot->len++;
}

optable* optable_new() {
    optable* ot = malloc(sizeof(optable));
    ot->optable = malloc(sizeof(wordop) * OPTABLE_MAX_SIZE);
    int inittablesize = sizeof(inittable);
    ot->len = inittablesize / sizeof(*inittable);
    memcpy(ot->optable, inittable, inittablesize);

    typedef struct {
        char* name;
        int len;
        char** words;
    } tocompile;
    tocompile defs[] = {
        {"nip", 2, (char*[]){"swap","drop"}},
        {"tuck", 3, (char*[]){"dup", "rot", "rot"}},
        {"incr", 2, (char*[]){"1", "+"}},
    };
    int defslen = sizeof(defs) / sizeof(*defs);
    for (int i = 0; i < defslen; i++) {
        tocompile d = defs[i];
        char* nm = defs[i].name;
        char** ws = d.words;
        optable_addop(ot,nm, d.len,ws);
    }
    return ot;
}

wordop* optable_getop(optable* optable, char *word) {
    for (int i = 0; i < optable->len; i++) {
        if (!strcmp(optable->optable[i].word, word)) {
            return &optable->optable[i];
        }
    }
    return 0;
}

/* Implementations of builtin functions */

static void not(stack *s) {
    int x = stack_pop(s);
    stack_push(s, !x);
}

static void drop(stack *s) {
    stack_pop(s);
}

static void over(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, y);
    stack_push(s, x);
    stack_push(s, y);
}

static void rot(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    int z = stack_pop(s);
    stack_push(s, y);
    stack_push(s, x);
    stack_push(s, z);
}

static void swap(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, x);
    stack_push(s, y);
}

static void eq(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, x == y);
}

static void add(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, x + y);
}

static void mult(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, x * y);
}

static void s_div(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, y / x);
}

static void mod(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, y % x);
}

static void sub(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, y - x);
}

static void dup(stack *s) {
    int x = stack_pop(s);
    stack_push(s, x);
    stack_push(s, x);
}


static void popout(stack *s) {
#ifdef __EMSCRIPTEN__
        sprintf(outputbuffer, "%d\n", stack_pop(s));
        (*outputline)++;
#else
        printf("%d\n", stack_pop(s));
#endif
}

static void peekout(stack *s) {
    int x = stack_pop(s);
    stack_push(s, x);
#ifdef __EMSCRIPTEN__
        sprintf(outputbuffer, "%d\n", x);
        (*outputline)++;
#else
        printf("%d\n", x);
#endif
}

static void donothing(stack *s) {
    // Do nothing at all (i.e. discard token)
}

static void depth(stack *s) {
    stack_push(s, stack_depth(s));
}

static void max(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    if (x > y) {
        stack_push(s, x);
    } else {
        stack_push(s, y);
    }
}

static void min(stack *s) {
    int x = stack_pop(s);
    int y = stack_pop(s);
    if (x < y) {
        stack_push(s, x);
    } else {
        stack_push(s, y);
    }
}

static void negate(stack *s) {
    stack_push(s, 0 - stack_pop(s));
}

static void s_abs(stack *s) {
    int x = stack_pop(s);
    stack_push(s, x < 0 ? 0 - x : x);
}

/* Directives */

static void ifdirective(stack *s, int len, char* line, int* starti, optable* ot_IDNORED) {
    int i = *starti;
    stackitem predicate = stack_pop(s);
    if (!predicate) {
        while (len > i && i >= 4 && !(
                    line[i-4] == 't' &&
                    line[i-3] == 'h' &&
                    line[i-2] == 'e' &&
                    line[i-1] == 'n'
                    )) {
            i++;
        }
    }
    *starti = i;
}


/**
 * defineop reads a function identifier, followed by the commands to run when the function
 *      is called, stopping when a semicolon is reached.
 *      Reading is done from the input string.
 *
 * returns new position of input index
 *
 */
static void defineop(stack* s_IGNORED, int len_IGNORED, char *input, int* starti, optable* optable) {
    // value easier to deal with (than pointer)
    int i = *starti;
    // name by which the function will be called
    char* opcode = malloc(sizeof(char) * WORD_LEN_LIMIT);
    int opcodei = 0;

    // code to be evaluated when function is called
    char* funcscript = malloc(sizeof(char) * DEFINED_FUNC_MAX_LENGTH);
    int funcscripti = 0;
    
    // skip ' ' and ':'
    while (input[i] == ' ' || input[i] == ':') {
        i++;
    }

    // get name
    while (input[i] != ' ' && opcodei < WORD_LEN_LIMIT) {
        opcode[opcodei++] = input[i++];
    }
    opcode[opcodei] = '\0';

    // get code
    while (input[i] != ';' && funcscripti < DEFINED_FUNC_MAX_LENGTH) {
        funcscript[funcscripti++] = input[i++];
    }
    funcscript[funcscripti] = '\0';

    // optable->optable bounds check 
    if (optable->len >= OPTABLE_MAX_SIZE) {
        // Error
        fprintf(stderr, "Error: optable->optable reached max size, failed to create new user defined operation");
        exit(1);
    }
    // add op to end of table, and increment size
    optable->optable[optable->len].word = opcode;
    optable->optable[optable->len].optype = script;
    optable->optable[optable->len].script = funcscript;
    optable->optable[optable->len].scriptlen = funcscripti;
    optable->len++;

    // move read position forwards
    *starti = i;
}
