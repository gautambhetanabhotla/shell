#include "seek.h"
#include "prompt.h"
#include "reveal.h"
#include "hop.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h>

char* path_wrt(char* path, char* wrt) {
    char* result = calloc(PATH_MAX, sizeof(char));
    snprintf(result, PATH_MAX, "./%s", path + strlen(wrt) + 1);
    // strcpy(result, "./");
    // strcat(result, path + strlen(wrt) + 1);
    return result;
}

void search_directory_r(char* parent, char* path, char* target, FILE* ostream, int* err, int* num_matches, char** match, bool d, bool f) {
    if(!parent || !path || !target || !ostream) return;
    DIR* dir = opendir(path);
    if(dir == NULL) {
        fprintf(stderr, "ERROR: Invalid path!\n");
        *err = -1;
        return;
    }
    struct dirent* entry;
    struct stat buf;
    while((entry = readdir(dir)) != NULL) {
        char entry_path[PATH_MAX];
        char* x = entry->d_name;
        if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
            continue;
        }
        sprintf(entry_path, "%s/%s", path, entry->d_name);
        if(lstat(entry_path, &buf)) {
            continue;
        }
        if(strstr(entry->d_name, target) == entry->d_name) {
            if(((S_ISDIR(buf.st_mode)) && f) || ((!(S_ISDIR(buf.st_mode))) && d)) continue;
            (*num_matches)++;
            *match = path_wrt(entry_path, parent);
            if((S_ISDIR(buf.st_mode))) fprintf(stdout, "\033[0;34m");
            else fprintf(stdout, "\033[0;32m");
            fprintf(stdout, "%s\033[0m\n", *match);
        }
        if((S_ISDIR(buf.st_mode))) search_directory_r(parent, entry_path, target, stdout, err, num_matches, match, d, f);
    }
    closedir(dir);
}

int seek(char** args) {
    bool d = false, e = false, f = false, target_acquired = false;
    int rc = 0;
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
            else {
                // parent = strdup(args[i]);
                parent = convert_path_back(args[i], false);
                if(parent == NULL) {
                    fprintf(stderr, "ERROR: Invalid path\n");
                    return -1;
                }
                if(args[i][0] != '/' && args[i][0] != '~' && args[i][0] != '-') {
                    strcpy(parent, CURRENT_DIRECTORY);
                    strcat(parent, "/");
                    strcat(parent, args[i]);
                }
            }
        }
        i++;
    }
    if(parent == NULL) {
        parent = calloc(PATH_MAX, sizeof(char));
        strcpy(parent, CURRENT_DIRECTORY);
    }
    // parent = realloc(parent, (PATH_MAX + 1));
    if(d && f) {
        fprintf(stderr, "ERROR: Invalid flags!\n");
        return -1;
    }
    int num_matches = 0;
    char* match = NULL;
    if(target || parent) search_directory_r(parent, parent, target, stdout, &rc, &num_matches, &match, d, f);
    else fprintf(stderr, "ERROR: Invalid arguments\n");
    if(num_matches == 0 && (target || parent)) {
        fprintf(stdout, "No matches!\n");
    }
    else if((num_matches == 1) && e) {
        struct stat buf;
        char shit2[PATH_MAX];
        sprintf(shit2, "%s/%s", parent, match + 2);
        if(lstat(shit2, &buf)) {
            fprintf(stderr, "Missing permissions for task!\n");
            return -1;
        }
        if((!S_ISDIR(buf.st_mode))) {
            FILE* file = fopen(shit2, "r");
            while(!feof(file)) {
                fgets(shit2, 4096, file);
                fprintf(stdout, "%s", shit2);
            }
            fprintf(stdout, "\n");
        }
        else {
            char* args[] = {"hop", shit2, NULL};
            if(hop(args)) {
                fprintf(stderr, "ERROR: Could not switch directory\n");
            }
        }
    }
    return rc;
} 