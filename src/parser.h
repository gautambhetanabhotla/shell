#include <stdbool.h>

#ifndef PARSER_H
#define PARSER_H

struct command {
    char* string;
    bool background, receiving_pipe, sending_pipe;
};

struct command* separate_commands(char* input);
char** get_args(char* input, bool background);
char* convert_path(char* input_string, char* home_dir, bool free_input);

#endif
