#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

struct command* separate_commands(char* input) {
    char* Input = (char*) malloc(strlen(input) + 1);
    struct command* commands = malloc(sizeof(struct command));
    int i = 0;
    strcpy(Input, input);
    char *l = Input, *r = Input;
    while(*r) {
        while(*r != '&' && *r != ';' && *r) r++;
        if(*r == '&') {
            *r = '\0';
            r++;
            commands = realloc(commands, (i+1) * sizeof(struct command));
            commands[i].string = (char*) malloc(strlen(l) + 1);
            strcpy(commands[i].string, l);
            commands[i].background = true;
            i++;
            l = r;
        }
        else if(*r == ';') {
            *r = '\0';
            r++;
            commands = realloc(commands, (i+1) * sizeof(struct command));
            int frgfdj = strlen(l);
            commands[i].string = (char*) malloc(strlen(l) + 1);
            strcpy(commands[i].string, l);
            commands[i].background = false;
            i++;
            l = r;
        }
        else if(*r == '\0') {
            if(l - r == 1) break;
            char* x = l;
            bool not_only_whitespaces = false;
            while(*x) {
                if(*x != ' ' && *x != '\t') not_only_whitespaces = true;
                x++;
            }
            if(not_only_whitespaces) {
                commands = realloc(commands, (i+1) * sizeof(struct command));
                commands[i].string = (char*) malloc(strlen(l) + 1);
                strcpy(commands[i].string, l);
                commands[i].background = false;
                i++;
            }
            else break;
        }
        else {
            fprintf(stderr, "\nWhy are we here?\n");
            exit(1);
        }
    }
    // i++;
    commands = realloc(commands, sizeof(struct command*) * (i));
    commands[i].string = NULL;

    #ifdef DEBUG
        printf("Commands:\n");
        for(int j = 0; j < i; j++) if(commands[j].string) printf("%s: %s\n", commands[j].string, commands[j].background ? "Background" : "Foreground");
    #endif

    return commands;
}

char** get_args(char* input, bool background) {
    char* Input = (char*) malloc((strlen(input) + 1) * sizeof(char));
    strcpy(Input, input);
    char* saveptr;
    char** args = NULL;
    int i = 0;
    char* arg = __strtok_r(Input, " ", &saveptr);
    while(arg != NULL) {
        args = realloc(args, sizeof(char*) * (i + 1));
        args[i] = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(args[i], arg);
        arg = __strtok_r(NULL, " ", &saveptr);
        i++;
    }
    if(background) {
        args = realloc(args, sizeof(char*) * (i + 1));
        args[i] = (char*) malloc(sizeof(char) * 2);
        strcpy(args[i], "&"); // based on what exec() calls assume background
        // arg = __strtok_r(NULL, " ", &saveptr);
        i++;
    }
    args = realloc(args, sizeof(char*) * (i + 1));
    args[i] = NULL;

    #ifdef DEBUG
        printf("Args:\n");
        for(int j = 0; j < i; j++) if(args[j]) printf("%s\n", args[j]);
    #endif

    return args;
}

void run(char* Command) {
    char* command = malloc(sizeof(char) * (strlen(Command) + 1));
    strcpy(command, Command);
    int l = strlen(command);
    int command_match = -1;

    char** args = NULL;
    int i = 0;
    char* arg = strtok(command, " ");
    while(arg != NULL) {
        args = realloc(args, sizeof(char*) * (i + 1));
        args[i] = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(args[i], arg);
        arg = strtok(NULL, " ");
        i++;
    }

    #ifdef DEBUG
        printf("Args:\n");
        for(int j = 0; j < i; j++) printf("%s\n", args[j]);
    #endif
}

void take_input() {
    char* saveptr;

    char query[4097] = {'\0'};
    fgets(query, 4096, stdin);
    query[strlen(query) - 1] = '\0'; // getting rid of the newline at the end
    #ifdef DEBUG
        printf("Query: %s\n", query);
    #endif

    char* command = strtok_r(query, ";", &saveptr);
    while(command != NULL) {
        run(command);
        command = strtok_r(NULL, ";", &saveptr);
    }
}