#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

struct command* realloc_commands(struct command* input, int size) {
    struct command* result = malloc((size + 1) * sizeof(struct command));
    if(input == NULL) return result;
    if(result == NULL) return NULL;
    for(int i = 0; i < size; i++) {
        result[i].string = input[i].string;
        result[i].background = input[i].background;
        result[i].receiving_pipe = input[i].receiving_pipe;
        result[i].sending_pipe = input[i].sending_pipe;
    }
    // for(int i = 0; i < size; i++) if(input[i].string) free(input[i].string);
    free(input);
    return result;
}

bool only_whitespace(char* str) {
    char* x = str;
    while(*x) {
        if(*x != ' ' && *x != '\t') return false;
        x++;
    }
    return true;
}

struct command* separate_commands(char* input) {
    if(input == NULL) return NULL;
    char* Input = (char*) malloc(strlen(input) + 1);
    if(Input == NULL) return NULL;
    // struct command* commands = malloc(sizeof(struct command));
    struct command* commands = NULL;
    int i = 0;
    strcpy(Input, input);
    char *l = Input, *r = Input;
    bool receiving_pipe = false;
    while(*r) {
        while(*r != '&' && *r != ';' && *r != '|' && *r) r++;
        if(*r == '&') {
            *r = '\0';
            r++;
            if(!only_whitespace(l)) {
                commands = realloc_commands(commands, (i+1));
                commands[i].string = (char*) malloc(strlen(l) + 1);
                strcpy(commands[i].string, l);
                commands[i].background = true;
                if(receiving_pipe) commands[i].receiving_pipe = true;
                commands[i].sending_pipe = false;
                receiving_pipe = false;
                i++;
            }
            l = r;
        }
        else if(*r == ';') {
            *r = '\0';
            r++;
            if(!only_whitespace(l)) {
                commands = realloc_commands(commands, (i+1));
                commands[i].string = (char*) malloc(strlen(l) + 1);
                strcpy(commands[i].string, l);
                commands[i].background = false;
                if(receiving_pipe) commands[i].receiving_pipe = true;
                commands[i].sending_pipe = false;
                receiving_pipe = false;
                i++;
            }
            l = r;
        }
        else if(*r == '|') {
            *r = '\0';
            r++;
            if(!only_whitespace(l)) {
                commands = realloc_commands(commands, (i+1));
                commands[i].string = (char*) malloc(strlen(l) + 1);
                strcpy(commands[i].string, l);
                commands[i].background = false;
                if(receiving_pipe) commands[i].receiving_pipe = true;
                receiving_pipe = false;
                commands[i].sending_pipe = true;
                receiving_pipe = true;
                i++;
            }
            receiving_pipe = true;
            l = r;
        }
        else if(*r == '\0') {
            if(!only_whitespace(l)) {
                commands = realloc_commands(commands, (i+1));
                commands[i].string = (char*) malloc(strlen(l) + 1);
                strcpy(commands[i].string, l);
                commands[i].background = false;
                if(receiving_pipe) commands[i].receiving_pipe = true;
                commands[i].sending_pipe = false;
                receiving_pipe = false;
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
    commands = realloc_commands(commands, i);
    commands[i].string = NULL;

    #ifdef DEBUG
        printf("Commands:\n");
        for(int j = 0; j < i; j++) if(commands[j].string) printf("%s: %s, %s, %s\n", commands[j].string, commands[j].background ? "Background" : "Foreground", commands[j].receiving_pipe ? "Receiving Pipe" : "Not Receiving Pipe", commands[j].sending_pipe ? "Sending Pipe" : "Not Sending Pipe");
    #endif

    return commands;
}

char** get_args(char* input, bool background) {
    if(input == NULL) return NULL;
    char* Input = (char*) calloc((strlen(input) + 1), sizeof(char));
    strcpy(Input, input);
    char* saveptr;
    char** args = NULL;
    int i = 0;
    char* arg = __strtok_r(Input, " ", &saveptr);
    while(arg != NULL) {
        args = realloc(args, sizeof(char*) * (i + 1));
        args[i] = (char*) calloc((strlen(arg) + 1), sizeof(char));
        strcpy(args[i], arg);
        arg = __strtok_r(NULL, " ", &saveptr);
        i++;
    }
    // if(background) {
    //     args = realloc(args, sizeof(char*) * (i + 1));
    //     args[i] = (char*) calloc(sizeof(char) * 2, sizeof(char));
    //     strcpy(args[i], "&"); // based on what exec() calls assume background
    //     // arg = __strtok_r(NULL, " ", &saveptr);
    //     i++;
    // }
    args = realloc(args, sizeof(char*) * (i + 1));
    args[i] = NULL;

    #ifdef DEBUG
        printf("Args:\n");
        for(int j = 0; j < i; j++) if(args[j]) printf("%s\n", args[j]);
    #endif

    return args;
}