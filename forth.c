#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include "stack.h"
#include "forthmachine.h"

int initialised = false;
forthmachine* fm;
int lastline = 0;
char* buffer_eval(int len, char* line) {
    if (!initialised) {
        fm = forthmachine_new();
        initialised = true;
    }
    strcpy(fm->outputbuffer, "");
    forthmachine_eval(fm, len, line);
    if (fm->outputbuffer && strlen(fm->outputbuffer)) {
        return fm->outputbuffer;
    } else {
        return "";
    }
}

void stdin_eval() {
    forthmachine* fm = forthmachine_new();
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        forthmachine_eval(fm, len, line);
        printf("%s", fm->outputbuffer);
        strcpy(fm->outputbuffer, "");
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
