#include "seek.h"
#include "prompt.h"
#include "reveal.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

void search_directory_r(char* path) {
    DIR* dir = opendir(path);
}

int seek(char** args, FILE* istream, FILE* ostream) {
    bool d = false, e = false, f = false, target_acquired = false;
    char *target = NULL, *parent = NULL;
    int i = 1;
    while(args[i]) {
        if(args[i][0] == '-' && strlen(args[i]) != 1) {
            //interpret flags
            for(int j = 1; args[i][j]; j++) {
                if(args[i][j] == 'd') d = true;
                if(args[i][j] == 'e') e = true;
                if(args[i][j] == 'f') f = true;
            }
        }
        else {
            if(!target_acquired) {
                target = strdup(args[i]);
                target_acquired = true;
            }
            else parent = strdup(args[i]);
        }
        i++;
    }
    if(parent == NULL) parent = strdup(CURRENT_DIRECTORY);
    if(d && f) {
        fprintf(stderr, "ERROR: Invalid flags!\n");
        return -1;
    }
} 