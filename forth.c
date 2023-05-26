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

void exec(stack *s, char *word) {
    wordop* op = getop(word);
    if (op) {
        if (op->isscript) {
            eval(s, op->scriptlen, op->script);
        } else {
            op->op(s);
        }
        return;
    }
    if (isnumber(word)) {
        push(s, atoi(word));
        return;
    }
}

void eval(stack* s, int len, char* line) {
    char word[WORD_LEN_LIMIT];
        int wordi = 0;
        for (int i = 0; i < len; i++) {
            // end of input is \n, char after end of input is \0
            if (line[i] == '\0') {
                return;
            }
            if (line[i] != ' ' &&
                    line[i] != '\n' && wordi < WORD_LEN_LIMIT - 1) {
                if (line[i] == ':') {
                    i = defineop(i, line);
                    wordi = 0;
                } else {
                    word[wordi++] = line[i];
                }
            } else { // end of word
                if (wordi > 0) { // don't exec an empty string
                    word[wordi] = '\0';
                    if (!strcmp(word, "if")) {
                        stackitem predicate = pop(s);
                        if (!predicate) {
                            while (len > i && !(
                                        line[i-3] == 't' &&
                                        line[i-2] == 'h' &&
                                        line[i-1] == 'e' &&
                                        line[i] == 'n'
                                        )) {
                                i++;
                            }
                        }
                    } else {
                        exec(s, word);
                    }
                }
                // start new word
                wordi = 0;
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
