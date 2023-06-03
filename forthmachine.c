#include "forthmachine.h"
#include <stdio.h>
#include <string.h>
#include "drhstrings.h"
#include "stack.h"
/****/

forthmachine* forthmachine_new() {
    forthmachine* fm = (forthmachine*)malloc(sizeof(forthmachine));
    fm->ot = optable_new();
    fm->s = stack_new();
    fm->outputbuffer = (char*)malloc(sizeof(char) * MAX_OUTPUT_BUFFER_SIZE);
    strcpy(fm->outputbuffer, "");
    return fm;
}

static void op_exec(wordop* op, forthmachine* fm) {
    switch (op->optype) {
        case builtin:
            op->op(fm);
            break;
        case compiled:
            for (int j = 0; j < op->oplistlen; j++) {
                switch (op->oplist[j].type) {
                    case compileditem_literal:
                        stack_push(fm->s, op->oplist[j].literal);
                        break;
                    case compileditem_stackop:
                        op_exec(op->oplist[j].wordop, fm);
                        break;
                    case compileditem_ifcontrol:
                        {
                            stackitem si = stack_pop(fm->s);
                            int jumpto = op->oplist[j].jumpto;
                            if (!si && jumpto != -1) {
                                j = jumpto - 1;
                            }
                            break;
                        }
                }
            }
            break;
    }
}

static void forthmachine_exec(forthmachine* fm, char *word, int len, char* line, int* i) {
    if (0 == strcmp(word, ":")) {
        optable_defineop(fm->ot, line, i);
        return;
    }
    wordop* op = optable_getop(fm->ot, word);
    if (op) {
        op_exec(op, fm);
    } else if (isnumber(word)) {
        stack_push(fm->s, atoi(word));
    }
}

void forthmachine_eval(forthmachine* fm, int len, char* line) {
    char word[WORD_LEN_LIMIT];
    int wordi = 0;
    for (int i = 0; i < len; i++) {
        if (notdelim(line[i]) && wordi < WORD_LEN_LIMIT - 1) {
            word[wordi++] = line[i];
        } else { // end of word
            if (wordi > 0) { // don't exec an empty string
                word[wordi] = '\0';
                forthmachine_exec(fm, word, len, line, &i);
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
