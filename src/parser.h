#include <stdbool.h>

#ifndef PARSER_H
#define PARSER_H

struct command {
    char* string;
    bool background, receiving_pipe, sending_pipe;
};

struct command* separate_commands(char* input);
char** get_args(char* input, bool background);

#endif
