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

typedef struct {
    bool isliteral;
    union {
        wordop* wordop;
        stackitem literal;
    };
} compileditem;

typedef enum {
    directive = 0,
    builtin = 1,
    script = 2,
    compiled = 3,
} optype;

struct wordop {
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
