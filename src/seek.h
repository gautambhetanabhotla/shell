#include <stdio.h>
#include <stdbool.h>

#ifndef SEEK_H
#define SEEK_H

int seek(char** args, FILE* istream, FILE* ostream);
void search_directory_r(char* parent, char* path, char* target, FILE* ostream, int* err, int* num_matches, char** match, bool d, bool f);

#endif