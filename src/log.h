#include <stdio.h>

#ifndef LOG_H
#define LOG_H

extern char* PAST_COMMANDS[15];
extern int command_head;

int init_log(), save_log(), purge_log(), add_to_log(char*), output_log(), Log(char** args);

#endif