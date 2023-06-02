#include "forthmachine.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drhstrings.h"

static void not(forthmachine* fm);
static void drop(forthmachine* fm);
static void over(forthmachine* fm);
static void rot(forthmachine* fm);
static void swap(forthmachine* fm);
static void eq(forthmachine* fm);
static void add(forthmachine* fm);
static void mult(forthmachine* fm);
static void s_div(forthmachine* fm);
static void mod(forthmachine* fm);
static void negate(forthmachine* fm);
static void s_abs(forthmachine* fm);
static void max(forthmachine* fm);
static void min(forthmachine* fm);
static void sub(forthmachine* fm);
static void dup(forthmachine* fm);
static void popout(forthmachine* fm);
static void peekout(forthmachine* fm);
static void donothing(forthmachine* fm);
static void depth(forthmachine* fm);

static void printall(forthmachine* fm);
static void pick(forthmachine* fm);
static void roll(forthmachine* fm);
static void clearstack(forthmachine* fm);

static void ifdirective(forthmachine* fm, int len, char* line, int* i);
static void defineop(forthmachine* fm, int len, char* line, int* i);

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
    {"pick", builtin, {pick}},
    {"roll", builtin, {roll}},
    {"then", builtin, {donothing}},
    {"depth", builtin, {depth}},
    {".s", builtin, {printall}},
    {"clearstack", builtin, {clearstack}},
    {"if", directive, {ifdirective}},
    {":", directive, {defineop}},
};
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

static void not(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    stack_push(s, !x);
}

static void drop(forthmachine* fm) {
    stack* s = fm->s;
    stack_pop(s);
}

static void over(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, y);
    stack_push(s, x);
    stack_push(s, y);
}

static void rot(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    int z = stack_pop(s);
    stack_push(s, y);
    stack_push(s, x);
    stack_push(s, z);
}

static void swap(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, x);
    stack_push(s, y);
}

static void eq(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, x == y);
}

static void add(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, x + y);
}

static void mult(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, x * y);
}

static void s_div(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, y / x);
}

static void mod(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, y % x);
}

static void sub(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    stack_push(s, y - x);
}

static void dup(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    stack_push(s, x);
    stack_push(s, x);
}


static void forthmachine_output(forthmachine* fm, stackitem v) {
if (fm->outputbuffer) {
    char x[WORD_LEN_LIMIT];
    sprintf(x, "%d\n", v);
    strcat(fm->outputbuffer, x); // TODO: fix: UNSAFE!
} else {
    printf("%d\n", v);
}
}

static void popout(forthmachine* fm) {
    stack* s = fm->s;
    forthmachine_output(fm, stack_pop(s));
}

static void peekout(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    stack_push(s, x);
    forthmachine_output(fm, x);
}

static void donothing(forthmachine* fm) {
    // Do nothing at all (i.e. discard token)
}

static void depth(forthmachine* fm) {
    stack* s = fm->s;
    stack_push(s, stack_depth(s));
}

static void max(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    if (x > y) {
        stack_push(s, x);
    } else {
        stack_push(s, y);
    }
}

static void min(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    int y = stack_pop(s);
    if (x < y) {
        stack_push(s, x);
    } else {
        stack_push(s, y);
    }
}

static void negate(forthmachine* fm) {
    stack* s = fm->s;
    stack_push(s, 0 - stack_pop(s));
}

static void s_abs(forthmachine* fm) {
    stack* s = fm->s;
    int x = stack_pop(s);
    stack_push(s, x < 0 ? 0 - x : x);
}

static void printall(forthmachine* fm) {
    stack_tostringappend(fm->s, MAX_OUTPUT_BUFFER_SIZE, fm->outputbuffer);
}

static void pick(forthmachine* fm) {
    stack_pick(fm->s);
}
static void roll(forthmachine* fm) {
    stack_roll(fm->s);
}
static void clearstack(forthmachine* fm) {
    stack_clear(fm->s);
}

/* Directives */

static void ifdirective(forthmachine* fm, int len, char* line, int* starti) {
    stack* s = fm->s;
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
static void defineop(forthmachine* fm, int len_IGNORED, char *input, int* starti) {
    optable* optable = fm->ot;
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
