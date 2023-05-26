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

//https://stackoverflow.com/a/58585995
char isnumber(char *text) {
    int j;
    j = strlen(text);
    while(j--) {
        if(text[j] >= '0' && text[j] <= '9')
            continue;
        return 0;
    }
    return 1;
}

void eval(stack* s, int len, char* line);

void exec(stack *s, char *word, int len, char* line, int* i) {
    wordop* op = getop(word);
    if (op) {
        if (op->optype == script) {
            eval(s, op->scriptlen, op->script);
        } else if (op->optype == builtin) {
            op->op(s);
        } else if (op->optype == directive) {
            op->directive(s, len, line, i);
        }
    } else if (isnumber(word)) {
        push(s, atoi(word));
    }
}

bool notdelim(char c) {
    return c != ' ' && c != '\n' && c != '\t' && c != '\0';
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
