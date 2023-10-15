#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/wait.h>
#include "myHelp.h"
#include "job.h"

extern Job *job_list;
extern int  global_job_size;

void childHandler(int signo) {
    raise(SIGTSTP);
}


void execDir(char** cmd, char* input, char* output, int inType, int outType, int p_fd[][2], int index, int pipes) {
    
    signal(SIGINT, childHandler);
    signal(SIGQUIT, childHandler);

    int fd;
    if(input!=NULL) {
        // <
        if(inType==0) {
            
            fd = open(input, O_RDONLY);

            if(fd == -1) {
                //printf("invalid file\n");
                exit(100);
            }
            if (dup2(fd, STDIN_FILENO) == -1) {
                //printf("invalid file\n");
                exit(100);
            }
        }

    }
    if(output!=NULL) {

        // >
        if(outType==0) {

            fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                //printf("invalid file\n");
                exit(100);
            }

            if (dup2(fd, STDOUT_FILENO) == -1) {
                //printf("invalid file\n");
                exit(100);
            }

        // >>
        } else if(outType==2) {

            fd = open(output, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                //printf("invalid file\n");
                exit(100);
            }

            if (dup2(fd, STDOUT_FILENO) == -1) {
                //printf("invalid file\n");
                exit(100);
            }
        }
    }

    
    if(outType==1) {
        dup2(p_fd[index][1], STDOUT_FILENO);
    } 
    if(inType==1) {
        dup2(p_fd[index-1][0], STDIN_FILENO);
    }

    for(int i=0;i<pipes;i++) {
        close(p_fd[i][0]);
        close(p_fd[i][1]);
    }

    execvp(cmd[0], cmd);
    
    exit(111);
    
    // if(pid>0) {
    //     int status = 0;
    //     wait(&status); 

    //         if (WIFEXITED(status)) {
    //         if(pipes>0) {
    //             if(index!=0) close(p_fd[index-1][0]);
    //             if(index!=pipes) close(p_fd[index][1]);
    //         }     

    //         if(WEXITSTATUS(status) == 111) {
    //             return 1;
    //         } 
    //         if(WEXITSTATUS(status) == 100) {

    //             return 2;
    //         }

    //     } else if(WIFSIGNALED(status)) {

    //         if(WTERMSIG(status)==SIGINT || WTERMSIG(status)==SIGQUIT || WTERMSIG(status)==SIGTSTP) {
    //             addJob(pid, cmd_name);
    //         }
    //     }

    //     return 0;
    // } 

}


int execInCmd(char* input) {

    int result = 0;

    char* space = strchr(input, ' ');
    char* exec_parameter[3];
    int size = 0;

    if(space==NULL) {
        size = 2;
    } else {
        size = 3;
    }

    exec_parameter[size-1] = NULL;
    char* walk = strtok(input, " ");

    exec_parameter[0] = walk; 

    walk = strtok(NULL, " ");
    if(walk) exec_parameter[1] = walk;
    
    if(!strcmp(exec_parameter[0], "cd")) {
        myCd(exec_parameter);
    } else if(!strcmp(exec_parameter[0], "exit")) {
        myExit();
    } else if(!strcmp(exec_parameter[0], "jobs")) {
        myJobs();
    } else if(!strcmp(exec_parameter[0], "fg")) {
        myFg(exec_parameter);
    } 

    return result;
}