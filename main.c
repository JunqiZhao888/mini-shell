#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include "myHelp.h"
#include "job.h"

Job *job_list = NULL;
int  global_job_size = 0;

Job* addJob(int pid, char *command) {
    Job *new_job = (Job *)malloc(sizeof(Job));
    if (!new_job) {
        perror("memory allocation error");
        return NULL;
    }

    new_job->pid = pid;
    new_job->command = strdup(command);
    if (!new_job->command) {
        perror("command duplication error");
        free(new_job);
        return NULL;
    }
    new_job->job_id = ++global_job_size; 
    new_job->next = NULL;

    if (!job_list) {
        job_list = new_job;
    } 
    else {
        Job *current = job_list;
        while (current->next) {
            current = current->next;
        }
        current->next = new_job;
    }

    // printf("[%d] %s (pid: %d)\n", new_job->job_number, new_job->command, new_job->pid);
    return new_job;
}

void removeJob(int pid) {
    Job *current = job_list, *prev = NULL;

    while (current) {
        if (current->pid == pid) {
            if (prev) {
                prev->next = current->next;
            } else {
                job_list = current->next;
            }

            global_job_size--;

            Job* temp_walk = current->next;

            while(temp_walk) {
                temp_walk->job_id--;
                temp_walk = temp_walk->next;
            }

            free(current->command);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
    fprintf(stderr, "Error: invalid job\n");
}


Job* findJob(int job_num) {
    for (Job *current = job_list; current; current = current->next) {
        if (current->job_id == job_num) {
            return current;
        }
    }
    fprintf(stderr, "Error: invalid job\n");
    return NULL;
}

int hasSuspendedJobs() {
    return job_list != NULL;
}

void printJobs() {
    Job *current = job_list;
    while (current) {
        printf("[%d] %s\n", current->job_id, current->command);
        current = current->next;
    }
}

void noHandler(int signo) {
    
}

// void mySigChild(int signo) {

//     int status;
//     int pid;

//     // Reap all the dead processes.
//     while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
//         // [Handle process termination as necessary for your shell...]
//     }

//     // Set the shell back as the foreground process group.
//     tcsetpgrp(STDIN_FILENO, getpgrp());
// }

int main() {

    // setpgid(0, 0);
    // signal(SIGCHLD ,mySigChild);

    signal(SIGINT, noHandler);
    signal(SIGQUIT, noHandler);
    signal(SIGTSTP, noHandler);
   

    while(1) {
        prompt();
        cmdDispatch();
    }

}
