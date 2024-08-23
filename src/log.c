#include "log.h"
#include "prompt.h"

#include <stdio.h>
#include <linux/limits.h>
#include <string.h>
#include <stdlib.h>

char* PAST_COMMANDS[15] = {NULL};
int COMMAND_HEAD = 0;

int init_log() {
    // for(int i = 0; i < 15; i++) PAST_COMMANDS[i] = (char*) malloc(MAX_COMMAND_LENGTH + 1);
    char log_path[PATH_MAX];
    strcpy(log_path, HOME_DIRECTORY);
    strcpy(log_path + strlen(HOME_DIRECTORY), "/log.txt");
    FILE* log_f = fopen(log_path, "r");
    if(log_f != NULL) {
        char string[MAX_COMMAND_LENGTH + 2];
        int i = 0;
        while(!feof(log_f) && i < 15) {
            fgets(string, MAX_COMMAND_LENGTH + 1, log_f);
            if(strlen(string) == 0) {
                i++;
                continue;
            }
            string[strlen(string) - 1] = '\0';
            add_to_log(string);
            i++;
        }
    }
    else {
        log_f = fopen(log_path, "w");
        fclose(log_f);
    }
    return 0;
}

int save_log() {
    char log_path[PATH_MAX];
    strcpy(log_path, HOME_DIRECTORY);
    strcpy(log_path + strlen(HOME_DIRECTORY), "/log.txt");
    FILE* log_f = fopen(log_path, "w");
    if(log_f != NULL) {
        int n = 15;
        for(int i = COMMAND_HEAD; n; (i++ && n--)) {
            if(PAST_COMMANDS[i] == NULL) break;
            fprintf(log_f, "%s\n", PAST_COMMANDS[i]);
        }
    }
    else return -1;
    return 0;
}

int purge_log() {
    for(int i = 0; i < 15; i++) {
        if(PAST_COMMANDS[i]) free(PAST_COMMANDS[i]);
        PAST_COMMANDS[i] = NULL;
    }
    char log_path[PATH_MAX];
    strcpy(log_path, HOME_DIRECTORY);
    strcpy(log_path + strlen(HOME_DIRECTORY), "/log.txt");
    FILE* log_f = fopen(log_path, "w");
    fclose(log_f);
    return 0;
}

int add_to_log(char* query) {
    for(int i = 0; i < 15; i++) {
        if(PAST_COMMANDS[i] == NULL) {
            PAST_COMMANDS[i] = malloc(MAX_COMMAND_LENGTH + 1);
            strcpy(PAST_COMMANDS[i], query);
            return 0;
        }
    }
    strcpy(PAST_COMMANDS[COMMAND_HEAD], query);
    COMMAND_HEAD = (COMMAND_HEAD + 1) % 15;
    return 0;
}

int output_log() {
    int n = 15;
    for(int i = COMMAND_HEAD; n; (i++ && n--)) {
        if(PAST_COMMANDS[i] == NULL) break;
        printf("%s\n", PAST_COMMANDS[i]);
    }
    return 0;
}