#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "myHelp.h"

void prompt() {
    char cwd[100];
    // get the working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        // printf("Cur working dir not available\n");
        return;
    }

    char* last_part;
    
    // deal the root situation
    if(strcmp(cwd, "/")==0) {
        last_part = "/";
    } else {
        last_part = strrchr(cwd, '/') + 1;
    }

    char prompt[1024];
    sprintf(prompt, "[nyush %s]$ ", last_part);
    // flush, for fear fork copy
    printf("%s", prompt);
    fflush(stdout);
}