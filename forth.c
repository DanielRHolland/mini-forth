#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include "stack.h"
#include "forthmachine.h"

int exitcode = EXIT_SUCCESS;

// Only used when in buffer_eval mode.
int initialised = false;
// Only used when in buffer_eval mode.
forthmachine* fm;

// Pointer to errormessage.
// Only used when in buffer_eval mode.
char* errormessage;

void buffer_eval_handleerror(char* err) {
    exitcode = EXIT_FAILURE;
    // Set errormessage global
    errormessage = malloc(sizeof(char) * MAX_OUTPUT_BUFFER_SIZE);
    strcpy(errormessage, err);
    #ifdef __EMSCRIPTEN__
    initialised = false;
    free(fm);
    fm = NULL;
    strcat(errormessage, "\nEnvironment has been reset.\n");
    #endif
}


char* buffer_eval(int len, char* line) {
    if (!initialised) {
        // initialise global forthmachine instance on first call
        fm = forthmachine_new(buffer_eval_handleerror);
        initialised = true;
    }
    strcpy(fm->outputbuffer, "");
    forthmachine_eval(fm, len, line);
    if (exitcode == EXIT_FAILURE) {
        return errormessage;
    }
    if (fm->outputbuffer && strlen(fm->outputbuffer)) {
        return fm->outputbuffer;
    } else {
        return "";
    }
}

void error_print(char* error) {
    exitcode = exitcode || EXIT_FAILURE;
    fprintf(stderr, "%s\n", error);
    exit(exitcode);
}

void stdin_eval() {
    forthmachine* fm = forthmachine_new(error_print);
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
    if (argc>1) {
        printf("%s", buffer_eval(strlen(argv[1])+1, argv[1]));
    } else {
        stdin_eval();
    }
    return exitcode;
}

#else

#include <emscripten/emscripten.h>
#define EXTERN

EXTERN EMSCRIPTEN_KEEPALIVE char* extern_eval(char* line) {
    exitcode = EXIT_SUCCESS;
    return buffer_eval(strlen(line)+1, line);
}
#endif
