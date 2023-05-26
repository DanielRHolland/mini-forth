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

void eval(stack* s, int len, char* line);

void exec(stack *s, char *word, int len, char* line, int* i) {
    wordop* op = getop(word);
    if (op) {
        switch (op->optype) {
            case script:
                eval(s, op->scriptlen, op->script);
                break;
            case builtin:
                op->op(s);
                break;
            case directive:
                op->directive(s, len, line, i);
                break;
        }
    } else if (isnumber(word)) {
        push(s, atoi(word));
    }
}

void eval(stack* s, int len, char* line) {
    char word[WORD_LEN_LIMIT];
    int wordi = 0;
    for (int i = 0; i < len; i++) {
        if (notdelim(line[i]) && wordi < WORD_LEN_LIMIT - 1) {
            word[wordi++] = line[i];
        } else { // end of word
            if (wordi > 0) { // don't exec an empty string
                word[wordi] = '\0';
                exec(s, word, len, line, &i);
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

int main(int argc, char** argv) {
    stack* s = newstack();
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        eval(s, len, line);
    }
}
