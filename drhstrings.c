#include <string.h>
#include <stdbool.h>

bool isnumber(char* text) {
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
