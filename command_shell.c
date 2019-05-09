#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

// Function to print Current Directory. 
void printDir() 
{ 
    char cwd[1024]; 
     
    getcwd(cwd, sizeof(cwd)); 
    printf("\n%s:%s$:", getenv("USER"), cwd); 
} 

