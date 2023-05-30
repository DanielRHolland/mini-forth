#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#ifndef STACK_H
#include "stack.h"
#endif
#ifndef OPTABLE_H
#include "optable.h"
#endif

bool isnumber(char *text) {
    for(int j = strlen(text); j > 0; j--) {
        if(text[j] < '0' && text[j] > '9') {
            return false;
        }
    }
    return true;
}

bool notdelim(char c) {
    return c != ' ' && c != '\n' && c != '\t' && c != '\0';
}

void eval(optable* ot, stack* s, int len, char* line);

void op_exec(wordop* op, optable* ot, stack *s, char *word, int len, char* line, int* i) {
    switch (op->optype) {
        case script:
            eval(ot, s, op->scriptlen, op->script);
            break;
        case builtin:
            op->op(s);
            break;
        case directive:
            op->directive(s, len, line, i, ot);
            break;
        case compiled:
            for (int j = 0; j < op->oplistlen; j++) {
                if (op->oplist[j].isliteral) {
                    stack_push(s, op->oplist[j].literal);
                } else {
                    op_exec(op->oplist[j].wordop, ot, s, word, len, line, i);
                }
            }
    }
}

void exec(optable* ot, stack *s, char *word, int len, char* line, int* i) {
    wordop* op = optable_getop(ot, word);
    if (op) {
        op_exec(op, ot, s, word, len, line, i);
    } else if (isnumber(word)) {
        stack_push(s, atoi(word));
    }
}

void eval(optable* ot, stack* s, int len, char* line) {
    char word[WORD_LEN_LIMIT];
    int wordi = 0;
    for (int i = 0; i < len; i++) {
        if (notdelim(line[i]) && wordi < WORD_LEN_LIMIT - 1) {
            word[wordi++] = line[i];
        } else { // end of word
            if (wordi > 0) { // don't exec an empty string
                word[wordi] = '\0';
                exec(ot, s, word, len, line, &i);
            }
            // start new word
            wordi = 0;
        }
        // end of input string
        if (line[i] == '\0') {
            return;
        }
    }
}


#ifndef __EMSCRIPTEN__

int main(int argc, char** argv) {
    optable* ot = optable_new();
    stack* s = stack_new();
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        eval(ot, s, len, line);
    }
}

#else

#include <emscripten/emscripten.h>
int initialised = false;
optable* ot;
stack* s;
int lastline = 0;

#define EXTERN

EXTERN EMSCRIPTEN_KEEPALIVE char* extern_eval(int len, char* line) {
    if (!initialised) {
        ot = optable_new();
        s = stack_new();
        initialised = true;
        outputline = malloc(sizeof(int));
        *outputline = 0;
        // outputtobuffer = true;
    }
    eval(ot, s, len, line);
    if (*outputline != lastline) {
        lastline = *outputline;
        return outputbuffer;
    } else {
        return "";
    }

}
#endif
