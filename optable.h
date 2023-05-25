#include <stdio.h>
#include <stdbool.h>
#include "stack.h"

#ifndef OPTABLE_H
#define OPTABLE_H

#define OPTABLE_MAX_SIZE 1024
#define DEFINED_FUNC_MAX_LENGTH 1024
#define WORD_LEN_LIMIT 255

typedef void (*stackop)(stack *);

typedef struct {
    char* word;
    bool isscript;
    union {
        stackop op;
        struct {
            char* script;
            int scriptlen;
        };
    };
} wordop;

/**
 * defineop reads a function identifier, followed by the commands to run when the function
 *      is called, stopping when a semicolon is reached.
 *      Reading is done from the input string.
 *
 * returns new position of input index
 *
 */
int defineop(int starti, char *input);

/**
 * getop returns the first wordop in the optable which is called by the word given as a parameter
 *      if none exist, returns 0
 */
wordop* getop(char *word);

#endif //OPTABLE_H
