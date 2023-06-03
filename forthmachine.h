#ifndef OPTABLE_H
#define OPTABLE_H
#include <stdio.h>
#include <stdbool.h>
#include "stack.h"


#define OPTABLE_MAX_SIZE 1024
#define DEFINED_FUNC_MAX_LENGTH 1024
#define WORD_LEN_LIMIT 255

typedef struct optable optable;
typedef struct wordop wordop;
typedef struct forthmachine forthmachine;


typedef void (*stackop)(forthmachine* fm);
typedef void (*directiveop)(forthmachine* fm, int len, char* line, int* i);

extern char* outputbuffer;
extern int outputline;

typedef enum {
    compileditem_stackop = 0,
    compileditem_literal = 1,
    compileditem_ifcontrol = 2,
} compileditem_type;

typedef struct {
    compileditem_type type;
    union {
        wordop* wordop;
        stackitem literal;
        struct {
            int jumpto;
        };
    };
} compileditem;

typedef enum {
    builtin = 0,
    compiled = 1,
} optype;

struct wordop {
    char* word;
    optype optype;
    union {
        stackop op;
        struct {
            compileditem* oplist;
            int oplistlen;
        };
    };
};

struct optable {
    int len;
    wordop* optable;
};

/**
 * getop returns the first wordop in the optable which is called by the word given as a parameter
 *      if none exist, returns 0
 */
wordop* optable_getop(optable* optable, char *word);

void optable_defineop(optable* optable, char *input, int* starti);

optable* optable_new();

struct forthmachine {
    optable* ot;
    stack* s;
    char* outputbuffer;
};

#define MAX_OUTPUT_BUFFER_SIZE 1024

forthmachine* forthmachine_new();
void forthmachine_eval(forthmachine* fm, int len, char* line);

#endif //OPTABLE_H
