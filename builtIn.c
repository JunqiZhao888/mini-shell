#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include "myHelp.h"
#include "job.h"

void myCd(char *arg[]) {

    if(chdir(arg[1]) != 0) {
        fprintf(stderr, "Error: invalid directory\n");
    }

    // 验证更改后的当前工作目录
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) != NULL) {
       
    } else {
        fprintf(stderr, "Error: invalid command\n");
    }
}

void myExit() {

    if(hasSuspendedJobs()) {
        fprintf(stderr, "Error: there are suspended jobs\n");
        return;
    }

    exit(0);
}

void myJobs() {

    if(hasSuspendedJobs()) printJobs();

}

void myFg(char *arg[]) {

    int job_id = atoi(arg[1]);

    int shell_pgid = getpgrp();

    Job* waked = findJob(job_id);
    if(waked) {

        int pid = waked->pid;
        int status;

        // tcsetpgrp(STDIN_FILENO, pid);
//////////////////////////////////////////////

        kill(pid, SIGCONT);
        waitpid(pid, &status, WUNTRACED);

        if(WIFEXITED(status)) {
            if(WEXITSTATUS(status) == 111) {
                fprintf(stderr, "Error: invalid program\n");
            } else if(WEXITSTATUS(status) == 100) {
                fprintf(stderr, "Error: invalid file\n");
            }

            removeJob(pid);
        } else if(WIFSTOPPED(status)) {

            char cmd_name[200] = "";
            cmd_name[0] = 0;

            strcat(cmd_name, waked->command);

        
            removeJob(pid);
            addJob(pid, cmd_name); 

        }
    }

    tcsetpgrp(STDIN_FILENO, shell_pgid);
//////////////////////////////////////////////
    

}
