#include "prompt.h"
#include "parser.h"

#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

char *HOME_DIRECTORY = NULL, *USERNAME = NULL, *HOSTNAME = NULL, *CURRENT_DIRECTORY = NULL;
int (*Functions[])(char**) = {};

char* convert_path(char* input_string, char* home_dir, bool free_input) {
    // Converts a general path to one relative to the home directory.
    char* result = calloc(PATH_MAX, sizeof(char));
    if(strcmp(input_string, home_dir) == 0) {
        strcpy(result, "~");
    }
    else if(strstr(input_string, home_dir) != NULL) {
        strcpy(result, "~/");
        strcat(result, input_string + strlen(home_dir) + 1);
    }
    else {
        strcpy(result, input_string);
    }
    if(free_input) free(input_string);
    return result;
}

void prompt() {
	printf("\e[0;31m%s@%s:%s\e[0;37m ", USERNAME, HOSTNAME, CURRENT_DIRECTORY);
    char query[1025] = {'\0'};
    fgets(query, 1024, stdin);
    query[strlen(query) - 1] = '\0';
    struct command* commands = separate_commands(query);
    int i = 0;
    while(commands[i].string) {
        char** args = get_args(commands[i].string, commands[i].background);
        i++;
    }
}

char* current_directory() {
    char* result = calloc(PATH_MAX, sizeof(char));
    getcwd(result, PATH_MAX);
    return convert_path(result, HOME_DIRECTORY, true); // this needs to be freed later by caller.
}

void init_sysvars() {
    // Initialise the shell's home directory to the current working directory
    HOME_DIRECTORY = calloc(PATH_MAX, sizeof(char));
    CURRENT_DIRECTORY = calloc(PATH_MAX, sizeof(char));
    getcwd(HOME_DIRECTORY, PATH_MAX);
    CURRENT_DIRECTORY = current_directory();

    USERNAME = calloc(257, sizeof(char));
    getlogin_r(USERNAME, 256);

    HOSTNAME = calloc(254, sizeof(char));
    gethostname(HOSTNAME, 253);
}

void execute(char** args) {

}