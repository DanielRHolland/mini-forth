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
//        fprintf(stderr, "[popped %d from i=%d]", si,s->size);
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

void add(stack *s) {

    int x = pop(s);
    int y = pop(s);
 //   fprintf(stderr, "adding %d + %d",x,y);
    push(s, x + y);
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
    {"dup", false, s_dup},
    {"dump", false, dump},
};
int optablelen = 4;

const int DEFINED_FUNC_MAX_LENGTH = 1024;
const int WORD_LEN_LIMIT = 255;

int defineop(int starti, char *input) {
    char *funcscript = malloc(sizeof(char) * DEFINED_FUNC_MAX_LENGTH);
    int funcscripti = 0;
    char *opcode = malloc(sizeof(char) * WORD_LEN_LIMIT);
    int opcodei = 0;
    while (input[starti] == ' ' || input[starti] == ':') {
        starti++;
    }
    while (input[starti] != ' ' && opcodei < WORD_LEN_LIMIT) {
        opcode[opcodei++] = input[starti++];
    }
    opcode[opcodei] = '\0';
    while (input[starti] != ';' && funcscripti < DEFINED_FUNC_MAX_LENGTH) {
        funcscript[funcscripti++] = input[starti++];
    }
    funcscript[funcscripti] = '\0';
    if (optablelen >= OPTABLE_MAX_SIZE) {
        // Error
        fprintf(stderr, "Error: optable reached max size, failed to create new user defined operation");
        exit(1);
    }
    optable[optablelen].word = opcode; //{opcode, true, funcscript};
    optable[optablelen].isscript = true;
    optable[optablelen].script = funcscript;
    optable[optablelen].scriptlen = funcscripti;
//    fprintf(stderr, "defining op '%s' with script '%s' and length '%d'", opcode, funcscript, funcscripti);
//    fprintf(stderr, "defined op %s", optable[optablelen].word);
    optablelen++;
    return starti;
}

void donothing(stack* s) {}

wordop* getop(char *word) {
    for (int i = 0; i < optablelen; i++) {
//    fprintf(stderr, "['%s' = '%s'?]", optable[i].word, word);
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
//    fprintf(stderr, "[looking up op %s]", word);
    wordop* op = getop(word);
    if (op) {

//    fprintf(stderr, "[op %s]", op->word);
        if (op->isscript) {
//            fprintf(stderr, "eval: %s (len: %d)", op->script, op->scriptlen);
            eval(s, op->scriptlen, op->script);
        } else {
            op->op(s);
        }
        return;
    }
    if (isnumber(word)) {
 //   fprintf(stderr, "[number %s]", word);
        push(s, atoi(word));
        return;
    }
}

//void eval(int scriptlen, char* script) {
void eval(stack* s, int len, char* line) {
    char word[WORD_LEN_LIMIT];
        int wordi = 0;
        for (int i = 0; i < len; i++) {

            // end of input is \n, char after end of input is \0
            if (line[i] == '\0') {
                return;
            }

//            printf("i%d - %c;", i, line[i]);
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
                    exec(s, word);
                }
                // start new word
                wordi = 0;
            }
        }
}

int main(int argc, char** argv) {
    stack* s = newstack();

    for (int i = 1; i < argc; i++) {
        exec(s, argv[i]);
    }

//    char ch;
//    char word[WORD_LEN_LIMIT];
//    int wordi;
//    while (read(STDIN_FILENO, &ch, 1) > 0) {
//        if (ch != ' ' && wordi < WORD_LEN_LIMIT) {
//            word[wordi++] = ch;
//        } else {
//            word[wordi] = '\0';
//            exec(s, word);
//            wordi = 0;
//        }
//    }
//
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
   
    while ((read = getline(&line, &len, stdin)) != -1) {
 //               printf("%s",line);
 //               printf("%d",(int)len);
        eval(s, len, line);
    }

 //   push(s, 8);
 //   push(s, 9);

 //   printf("%d\n", pop(s));
 //   printf("%d\n", pop(s));
 //   printf("%d\n", pop(s));
 //   printf("%d\n", pop(s));
 //   push(s, 8);
 //   push(s, 9);
 //   add(s);
 //   dup(s);
 //   printf("%d\n", pop(s));
 //   push(s, 1);
 //   sub(s);
 //   printf("%d\n", peek(s));
 //   printf("%d\n", pop(s));

 //   stackop op = getop("+");
 //   if (op) {
 //       op(s);
 //   }


}
