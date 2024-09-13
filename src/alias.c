#include "alias.h"
#include "prompt.h"

#include <string.h>
#include <stdlib.h>

struct Alias* ALIASES = NULL;
int N_ALIASES = 0;

char* replace(char* input, struct Alias* aliases, bool free_input) {
    // Replaces all patterns in a string according to the null-alias-terminated aliases array.
    // a null alias is one whose name and replacement are null.
    char* str = strdup(input);
    char *result = calloc(MAX_COMMAND_LENGTH, sizeof(char));
    char *l, *r;
    l = str;
    int i = 0;
    while(aliases[i].name && aliases[i].replacement) {
        strcpy(str, input);
        r = strstr(str, aliases[i].name);
        while(r != NULL) {
            *r = '\0';
            strcat(result, l);
            strcat(result, aliases[i].replacement);
            r += strlen(aliases[i].name);
            l = r;
            r = strstr(l, aliases[i].name);
        }
        i++;
    }
    strcat(result, l);
    free(str);
    if(free_input) free(input);
    return result;
}

void add_alias(char* name, char* replacement) {
    ALIASES = realloc(ALIASES, (N_ALIASES + 2) * sizeof(struct Alias));
    ALIASES[N_ALIASES].name = strdup(name);
    ALIASES[N_ALIASES].replacement = strdup(replacement);
    N_ALIASES++;
    ALIASES[N_ALIASES].name = NULL;
    ALIASES[N_ALIASES].replacement = NULL;
}

void purge_aliases() {
    for(int i = 0; i < N_ALIASES; i++) {
        free(ALIASES[i].name);
        free(ALIASES[i].replacement);
    }
    free(ALIASES);
    ALIASES = NULL;
    N_ALIASES = 0;
}

int alias(char** args) {
    if(args == NULL || args[0] == NULL || args[1] == NULL || args[2] == NULL || args[3] == NULL || (strcmp(args[2], "=") != 0)) {
        fprintf(stderr, "Invalid arguments!\n");
        return -1;
    }
    int i = 3;
    char* replacement = calloc(MAX_COMMAND_LENGTH, sizeof(char));
    while(args[i]) {
        strcat(replacement, args[i]);
        strcat(replacement, " ");
        i++;
    }
    add_alias(args[1], replacement);
    free(replacement);
    
    return 0;
}