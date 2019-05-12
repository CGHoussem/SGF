#ifndef SHELL_UTILITY
#define SHELL_UTILITY

#include "constants.h"
#include "struct_SGF.h"

char* readline();
char** parse(char* input);
int executeLine(Disk disk, char* input);

#endif