#include <stdio.h>
#include <stdbool.h>
#include "stack.h"

#ifndef OPTABLE_H
#define OPTABLE_H

#define OPTABLE_MAX_SIZE 1024
#define DEFINED_FUNC_MAX_LENGTH 1024
#define WORD_LEN_LIMIT 255

typedef struct optable optable;

typedef void (*stackop)(stack *);
typedef void (*directiveop)(stack *, int len, char* line, int* i, optable* optable);

typedef struct {
    bool isliteral;
    union {
        stackop stackop;
        stackitem literal;
    };
} compileditem;

typedef enum {
    directive = 0,
    builtin = 1,
    script = 2,
    compiled = 3,
} optype;

typedef struct {
    char* word;
    optype optype;
    union {
        directiveop directive;
        stackop op;
        struct {
            char* script;
            int scriptlen;
        };
        struct {
            compileditem* oplist;
            int oplistlen;
        };
    };
} wordop;

struct optable {
    int len;
    wordop* optable;
};

/**
 * getop returns the first wordop in the optable which is called by the word given as a parameter
 *      if none exist, returns 0
 */
wordop* optable_getop(optable* optable, char *word);

optable* optable_init();

#endif //OPTABLE_H
