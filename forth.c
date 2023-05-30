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

int initialised = false;
optable* ot;
stack* s;
int lastline = 0;

char* buffer_eval(int len, char* line) {
    if (!initialised) {
        ot = optable_new();
        s = stack_new();
        initialised = true;
        outputline = 0;
    }
    else {
        free(outputbuffer);
    }
    outputbuffer = malloc(sizeof(char) * 1024);
    eval(ot, s, len, line);
    if (outputline) {
        outputline = 0;
        return outputbuffer;
    } else {
        char* v = "foo";
        char* v2 = "bar";
        strcat(v, v2);
        strcat(v, line);
        return v;
    }
}

void stdin_eval() {
    optable* ot = optable_new();
    stack* s = stack_new();
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        eval(ot, s, len, line);
    }
}

#ifndef __EMSCRIPTEN__

int main(int argc, char** argv) {
    if (argc>1) printf("%s", buffer_eval(strlen(argv[1])+1, argv[1]));
    else stdin_eval();
}

#else

#include <emscripten/emscripten.h>
#define EXTERN

EXTERN EMSCRIPTEN_KEEPALIVE char* extern_eval(char* line) {
    return buffer_eval(strlen(line)+1, line);
}
#endif
