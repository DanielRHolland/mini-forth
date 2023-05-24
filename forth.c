#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

typedef int stackitem;

typedef struct {
    int size;
    int maxsize;
    stackitem* start;
} stack;

stack* newstack() {
    stack* s = malloc(sizeof(stack));
    s->size = 0;
    s->maxsize = 1024;
    s->start = malloc(sizeof(stackitem) * s->maxsize);
    return s;
}

stackitem pop(stack* s) {
    if (s->size > 0) {
        s->size = s->size - 1;
        stackitem si = s->start[s->size];
        return si;
    } else {
        // tried to pop empty stack
        return 0;
    }
}

stackitem peek(stack* s) {
    if (s->size > 0) {
        return s->start[s->size - 1];
    } else {
        // tried to pop empty stack
        return 0;
    }
}

void dump(stack* s) {
    for (int i = 0; i < s->size; i++) {
        printf("%d,", s->start[i]);
    }
    printf("\n");
}


void push(stack *s, stackitem si) {
//        fprintf(stderr, "pushing %d", si);
    if (s->size >= s->maxsize) {
        fprintf(stderr, "Error Stack Overflow");
        exit(1);
    } else {
        s->start[s->size] = si;
        s->size = s->size + 1;
    }
}

void not(stack *s) {
    int x = pop(s);
    push(s, !x);
}

void drop(stack *s) {
    pop(s);
}

void over(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, y);
    push(s, x);
    push(s, y);
}

void rot(stack *s) {
    int x = pop(s);
    int y = pop(s);
    int z = pop(s);
    push(s, y);
    push(s, x);
    push(s, z);
}

void swap(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, x);
    push(s, y);
}

void eq(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, x == y);
}

void add(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, x + y);
}

void mult(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, x * y);
}

void s_div(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, y / x);
}

void sub(stack *s) {
    int x = pop(s);
    int y = pop(s);
    push(s, y - x);
}

void s_dup(stack *s) {
    int x = pop(s);
    push(s, x);
    push(s, x);
}

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

#define OPTABLE_MAX_SIZE 1024
wordop optable[OPTABLE_MAX_SIZE] = {
    {"+", false, add},
    {"-", false, sub},
    {"*", false, mult},
    {"/", false, s_div},
    {"dup", false, s_dup},
    {"dump", false, dump},
    {"not", false, not},
    {"=", false, eq},
    {"swap", false, swap},
    {"drop", false, drop},
    {"over", false, over},
    {"rot", false, rot},
};
int optablelen = 12;

const int DEFINED_FUNC_MAX_LENGTH = 1024;
const int WORD_LEN_LIMIT = 255;

int defineop(int starti, char *input) {
    // code to be evaluated when function is called
    char *funcscript = malloc(sizeof(char) * DEFINED_FUNC_MAX_LENGTH);
    int funcscripti = 0;
    
    // name by which the function will be called
    char *opcode = malloc(sizeof(char) * WORD_LEN_LIMIT);
    int opcodei = 0;
    
    // skip ' ' and ':'
    while (input[starti] == ' ' || input[starti] == ':') {
        starti++;
    }

    // get name
    while (input[starti] != ' ' && opcodei < WORD_LEN_LIMIT) {
        opcode[opcodei++] = input[starti++];
    }
    opcode[opcodei] = '\0';

    // get code
    while (input[starti] != ';' && funcscripti < DEFINED_FUNC_MAX_LENGTH) {
        funcscript[funcscripti++] = input[starti++];
    }
    funcscript[funcscripti] = '\0';

    // optable bounds check
    if (optablelen >= OPTABLE_MAX_SIZE) {
        // Error
        fprintf(stderr, "Error: optable reached max size, failed to create new user defined operation");
        exit(1);
    }
    // add op to end of table, and increment size
    optable[optablelen].word = opcode;
    optable[optablelen].isscript = true;
    optable[optablelen].script = funcscript;
    optable[optablelen].scriptlen = funcscripti;
    optablelen++;
    return starti;
}

wordop* getop(char *word) {
    for (int i = 0; i < optablelen; i++) {
        if (!strcmp(optable[i].word, word)) {
            return &optable[i];
        }
    }
    return 0;
}

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
    if (!strcmp(word, "pop") || !strcmp(word, ".")) {
        printf("%d\n", pop(s));
        return;
    } else if (!strcmp(word, "peek")) {
        printf("%d\n", peek(s));
        return;
    }
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
                            while (len > i &&
                                    !(
                                        line[i-3] == 't' &&
                                        line[i-2] == 'h' &&
                                        line[i-1] == 'e' &&
                                        line[i] == 'n'
                                     )) {
                                i++;
                            }
                            //      if (len < i + 3) {
                            //          // error no 'then' before EOL
                            //          exit(1);
                            //      }
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
