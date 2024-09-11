#include "log.h"
#include "prompt.h"
#include "parser.h"

#include <stdio.h>
#include <linux/limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

char* PAST_COMMANDS[15] = {NULL};
int COMMAND_HEAD = 0;

int init_log() {
    // for(int i = 0; i < 15; i++) PAST_COMMANDS[i] = (char*) malloc(MAX_COMMAND_LENGTH + 1);
    char log_path[PATH_MAX];
    snprintf(log_path, sizeof(log_path), "%s/log.txt", HOME_DIRECTORY);
    // strcpy(log_path, HOME_DIRECTORY);
    // strcpy(log_path + strlen(HOME_DIRECTORY), "/log.txt");
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
    snprintf(log_path, sizeof(log_path), "%s/log.txt", HOME_DIRECTORY);
    FILE* log_f = fopen(log_path, "w");
    if(log_f != NULL) {
        int n = 14;
        for(int i = COMMAND_HEAD; n; ((i = (i+1) % 15) && n--)) {
            if(i == 15) continue;
            if(PAST_COMMANDS[i] == NULL) break;
            fprintf(log_f, "%s\n", PAST_COMMANDS[i]);
        }
        fclose(log_f);
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
            if(i > 0 && PAST_COMMANDS[i-1] != NULL && strcmp(PAST_COMMANDS[i-1], query) == 0) {
                // if the command matches last command
            }
            else {
                PAST_COMMANDS[i] = malloc(MAX_COMMAND_LENGTH + 1);
                strcpy(PAST_COMMANDS[i], query);
            }
            return 0;
        }
    }
    if(strcmp(PAST_COMMANDS[((COMMAND_HEAD - 1) % 15) >= 0 ? ((COMMAND_HEAD - 1) % 15) : (((COMMAND_HEAD - 1) % 15) + 15)], PAST_COMMANDS[COMMAND_HEAD]) != 0) {
        strcpy(PAST_COMMANDS[COMMAND_HEAD], query);
        COMMAND_HEAD = (COMMAND_HEAD + 1) % 15;
    }
    return 0;
}

int output_log() {
    int n = 14;
    for(int i = COMMAND_HEAD; n; ((i = (i+1) % 15) && n--)) {
        if(PAST_COMMANDS[i] == NULL) break;
        printf("%s\n", PAST_COMMANDS[i]);
    }
    return 0;
}

int Log(char** args, FILE* istream, FILE* ostream) {
    if(args == NULL) return -1;
    if(args[1] == NULL) {
        output_log();
    }
    else if(strcmp(args[1], "purge") == 0) {
        purge_log();
    }
    else if(strcmp(args[1], "execute") == 0) {
        if(args[2] == NULL) {
            fprintf(stderr, "ERROR: Index not provided.\n");
            return -1;
        }
        else {
            int index = atoi(args[2]);
            int indexTBE = ((COMMAND_HEAD - index) % 15) >= 0 ? ((COMMAND_HEAD - index) % 15) : (((COMMAND_HEAD - index) % 15) + 15);
            int i, j = 0;
            for(i = 0; i < 15; i++) if(PAST_COMMANDS[i] == NULL) {
                j = 1;
                break;
            }
            if(j) indexTBE = i - index;
            if(indexTBE >= 15 || indexTBE < 0 || PAST_COMMANDS[indexTBE] == NULL) {
                fprintf(stderr, "ERROR: Invalid index\n");
                return -1;
            }
            char query[MAX_COMMAND_LENGTH + 2];
            strcpy(query, PAST_COMMANDS[indexTBE]);

            struct command* commands = separate_commands(query);
            if(strstr(query, "log") == NULL) add_to_log(query);
            i = 0;
            while(commands[i].string != NULL) {
                i++;
            }
            if(commands && commands[i-1].sending_pipe == true) {
                fprintf(stderr, "Invalid usage of pipe!\n");
                return -1;
            }
            else if(commands && commands[0].receiving_pipe == true) {
                fprintf(stderr, "Invalid usage of pipe!\n");
                return -1;
            }
            int pipes[i][2];
            FILE *istream = stdin, *ostream = stdout;
            j = 0;
            while(commands[j].string) {
                char** args = get_args(commands[j].string, commands[j].background);
                if(commands[j].sending_pipe) {
                    if(pipe(pipes[j])) {
                        fprintf(stderr, "Failed to create pipe!\n");
                        perror("pipe");
                        return -1;
                    }
                    ostream = fdopen(pipes[j][1], "w");
                }
                if(commands[j].receiving_pipe) {
                    istream = fdopen(pipes[j-1][0], "r");
                }
                execute(args, commands[j].background, istream, ostream);
                j++;
            }
            for(j = 0; j < i; i++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
                free(commands[j].string);
            }
            free(commands);
        }
    }
    return 0;
}