#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell_utility.h"
#include "command_shell.h"
#include "primitives.h"

char* readline(){
    char* input = malloc(sizeof(char) * SHELL_BUFFER_SIZE);
    int pos = 0;
    char c;
    while ((c = getchar()) != '\n' && c != EOF){
        input[pos++] = c;
    }
    input[pos] = '\0';
    return input;
}

char** parse(char* input){
    int i = 0;
    char** args = malloc(sizeof(char) * SHELL_BUFFER_SIZE);
    const char* space = " ";
    char* arg = strtok(input, space);
    while (arg != NULL){
        args[i++] = arg;
        arg = strtok(NULL, space);
    }
    args[i] = NULL;
    return args;
}

int executeLine(Disk disk, char* input){
    char** parsedInput = parse(input);
    if (strcmp(input, "mkdir") == 0){
        mkdir(parsedInput[1], &disk, disk.inodes);
        return 1;
    } else if (strcmp(input, "ls") == 0){
        ls(disk.inodes);
        return 1;
    
    } else if (strcmp(input, "cp") == 0){
        mycreate(parsedInput[1], &disk, disk.inodes);
        return 1;
    } else if (strcmp(input, "mv") == 0){
        printf("IN DEV..\n");
        return 1;
    } else if (strcmp(input, "rm") == 0){
        printf("IN DEV..\n");
        return 1;
    } else if (strcmp(input, "help") == 0){
        printf("Available commands: mkdir, touch, ls, cp, mv, rm, exit\n");
        return 1;
    } else if (strcmp(input, "exit") == 0){
        return 0;
    } else {
        printf("%s: command not found\n", input);
        return 1;
    }
}