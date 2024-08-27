#include "hop.h"
#include "prompt.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>

extern char* HOME_DIRECTORY;
char* LAST_DIRECTORY = NULL;

char* convert_path_back(char* input_string, bool free_input) {
    // returns the path after expanding the home directory or '-'
    if(!input_string) return NULL;
    if(strstr(input_string, "~") != NULL) {
        if(input_string[0] != '~') {
            if(free_input && input_string) free(input_string);
            return NULL;
        }
        else {
            char* result = malloc(strlen(HOME_DIRECTORY) + strlen(input_string));
            // strcpy(result, HOME_DIRECTORY);
            // strcat(result + strlen(HOME_DIRECTORY), "/");
            // strcpy(result + strlen(HOME_DIRECTORY) + 1, input_string + 2);
            sprintf(result, "%s/%s", HOME_DIRECTORY, input_string + 2);
            return result;
        }
    }
    else {
        if(strlen(input_string) == 1 && input_string[0] == '-') {
            char* result = NULL;
            if(LAST_DIRECTORY) result = strdup(LAST_DIRECTORY);
            return result;
        }
        else {
            char* result = strdup(input_string);
            return result;
        }
    }
}

int hop(char** args) {
    char* buffer = calloc(PATH_MAX + 1, sizeof(char));
    strcpy(buffer, CURRENT_DIRECTORY);
    
    int rc = 0;
    if(!args[1]) {
        rc = chdir(HOME_DIRECTORY);
        getcwd(CURRENT_DIRECTORY, PATH_MAX);
        printf("%s\n", CURRENT_DIRECTORY);
    }
    else {
        for(int i = 1; args[i]; i++) {
            if(!args[i]) break;
            char* path = convert_path_back(args[i], false);
            if(!path) {
                fprintf(stderr, "ERROR: Invalid path\n");
                return -1;
            }
            rc = chdir(path);
            getcwd(CURRENT_DIRECTORY, PATH_MAX);
            printf("%s\n", CURRENT_DIRECTORY);
            free(path);
            if(rc) {
                switch(errno) {
                    case EACCES:
                        fprintf(stderr, "ERROR: Permission denied!\n");
                        break;
                    case ENOENT:
                        fprintf(stderr, "ERROR: Path does not exist!\n");
                        break;
                    default:
                        fprintf(stderr, "ERROR: Other\n");
                        break;
                }
                break;
            }
        }
    }
    if(LAST_DIRECTORY) strcpy(LAST_DIRECTORY, buffer);
    else LAST_DIRECTORY = strdup(buffer);
    getcwd(CURRENT_DIRECTORY, PATH_MAX);
    // free(CURRENT_DIRECTORY_CONVERTED);
    CURRENT_DIRECTORY_CONVERTED = convert_path(CURRENT_DIRECTORY, HOME_DIRECTORY, false);
    return rc;
}