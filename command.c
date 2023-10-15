#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "myHelp.h"
#include "job.h"

void parseEachProgram(char* command, int cur_index, int pipes, int p_fd[][2]) {

    // char temp_name[300] = "";
    // temp_name[0] = 0;
    // char cmd_name[300] = "";
    // cmd_name[0] = 0;

    // char* dir = "/usr/bin/";
  
    // strcat(temp_name, command);


    char* small_block[100];
    char* walk = strtok(command, " ");

    small_block[0] = walk;
    int count = 1;
    walk = strtok(NULL, " ");

    while (walk) {
        small_block[count++] = walk;
        walk = strtok(NULL, " ");
    }

   
    int inre = 0;
    int outre_1 = 0;
    int outre_2 = 0;
    int first_re = 0;

    for (int i = 0; i < count; i++) {
       
        if (!strcmp(small_block[i], "<")) inre = i;
        else if (!strcmp(small_block[i], ">")) outre_1 = i;
        else if (!strcmp(small_block[i], ">>")) outre_2 = i;

        if (first_re==0 && (inre || outre_1 || outre_2)) first_re = i;
      
    }

    // if(first_re && strchr(temp_name, '/')==NULL) {
    //     strcat(cmd_name, dir);
    // }

    // strcat(cmd_name, temp_name);

    if (first_re == 0) first_re = count;

    char* exec_cmd[200];
    exec_cmd[first_re] = NULL;
    for (int i = 0; i < first_re; i++) exec_cmd[i] = small_block[i];

    if (pipes==0) {

        if (inre && outre_1) {
            execDir(exec_cmd, small_block[inre + 1], small_block[outre_1 + 1], 0, 0, NULL, 0, pipes);
        }
        else if (inre && outre_2) {
            execDir(exec_cmd, small_block[inre + 1], small_block[outre_2 + 1], 0, 2, NULL, 0, pipes);
        }
        else if (inre) {
            execDir(exec_cmd, small_block[inre + 1], NULL, 0, -1, NULL, 0, pipes);
        }
        else if (outre_1 || outre_2) {
            if (outre_1) execDir(exec_cmd, NULL, small_block[outre_1 + 1], -1, 0, NULL, 0, pipes);
            if (outre_2) execDir(exec_cmd, NULL, small_block[outre_2 + 1], -1, 2, NULL, 0, pipes);
        }
        else {
            execDir(exec_cmd, NULL, NULL, -1, -1, NULL, 0, pipes);
        }
       
    }
    else {

        if (cur_index == 0) {
            if (inre) {
                execDir(exec_cmd, small_block[inre + 1], NULL, 0, 1, p_fd, cur_index, pipes);
            }
            else {
                execDir(exec_cmd, NULL, NULL, -1, 1, p_fd, cur_index, pipes);
            }
        }
        else if (cur_index > 0 && cur_index < pipes) {
            execDir(exec_cmd, NULL, NULL, 1, 1, p_fd, cur_index, pipes);
        }
        else if (cur_index == pipes) {
            if (outre_1) {
                execDir(exec_cmd, NULL,  small_block[outre_1+1], 1, 0, p_fd, cur_index, pipes);
            }
            else if (outre_2) {
                execDir(exec_cmd, NULL, small_block[outre_2 + 1], 1, 2, p_fd, cur_index, pipes);
            }
            else {
                execDir(exec_cmd, NULL, NULL, 1, -1, p_fd, cur_index, pipes);
            }
        }
    }
}

int handleInput(char* command) {
  
    char* block[300];
    char* walk = strtok(command, "|");
    block[0] = walk;
    int count = 1;
    walk = strtok(NULL, "|");

    while (walk) {
        block[count++] = walk;
        walk = strtok(NULL, "|");
    }

    int pipes = count - 1;

    int p_fd[pipes][2];

    for(int i=0;i<pipes;i++) {
        if(pipe(p_fd[i])==-1) {
            printf("Pipe init error\n");
            exit(0);
        }
    }

    int pid = 0;

    int result = 0;

    char first_command[200] = "";
    char tmp_first_command[200] = "";
    first_command[0] = 0;
    tmp_first_command[0] = 0;
    strcat(tmp_first_command, block[0]);
    if(strchr(block[0], '/')==NULL) {
        strcat(first_command, "/usr/bin/");
    }
    strcat(first_command, tmp_first_command);


    for (int i = 0; i < count; i++) {
        pid = fork();
        if(pid==0) {
            // if(pipes>0 && i>0) close(p_fd[i-1][1]);
            parseEachProgram(block[i], i, pipes, p_fd);     
        } 
    }

    for(int i=0;i<pipes;i++) {
        close(p_fd[i][0]);
        close(p_fd[i][1]);
    }

    if(count>1) {

        for(int i=0; i < count; i++) { 

            int status = 0;
            wait(&status); 

            if (WIFEXITED(status)) {
            // if(pipes>0) {
            //     if(i!=0) close(p_fd[i-1][0]);
            //     if(i!=pipes) close(p_fd[i][1]);
            // }     

                if(WEXITSTATUS(status) == 111) {
                    result = 1;
                    break;
                } 
                if(WEXITSTATUS(status) == 100) {
                    result = 2;
                    break;
                }

            }
        } 
    } 
    
    else if(count==1) {
        int status;

        waitpid(pid, &status, WUNTRACED);

        // if(((status)==SIGINT || WTERMSIG(status)==SIGQUIT || WTERMSIG(status)==SIGTSTP)) {
        //     addJob(pid, first_command);
        // }

        if(WIFSTOPPED(status)) addJob(pid, first_command);
        else if (WIFEXITED(status)) {
            // if(pipes>0) {
            //     if(i!=0) close(p_fd[i-1][0]);
            //     if(i!=pipes) close(p_fd[i][1]);
            // }     

                if(WEXITSTATUS(status) == 111) {
                    result = 1;
                } 
                if(WEXITSTATUS(status) == 100) {
                    result = 2;
                }

            }
    } 
    
    

    return result;
}

int isRedirect(char* cur) {
    if (!strcmp(cur, "<") || !strcmp(cur, ">") || !strcmp(cur, ">>") || !strcmp(cur, "|")) return 1;
    return 0;
}

int isBuiltIn(char * cur) {
    if(!strcmp(cur, "exit") || !strcmp(cur, "jobs")) return 1;
    else if(!strcmp(cur, "cd") || !strcmp(cur, "fg")) return 2;
    return 0;
}

int verifyCommand(char* command_0) {

    char command[1000] = "";
    strcpy(command, command_0);

    char* temp_token = strtok(command, " ");

    char* blocks[300];
    int block_num = 0;

    while (temp_token != NULL) {
        blocks[block_num++] = temp_token;
        temp_token = strtok(NULL, " ");
    }


    blocks[block_num] = NULL;


    int first_pipe = 0;
    int pipe_index = 0;


    int input_redirect_index = 0;
    int output_redirect_index = 0;
    int last_was_redirect = 0;

    int check_built_in = 0;

    for (int i = 0; i < block_num; i++) {

        if ((i == 0 || i == block_num - 1) && isRedirect(blocks[i])) {
                //printf("Redirect cannot be at the start or the end\n");
                return 0;       
        }

        
        if(i==0) {
            int temp_flag = isBuiltIn(blocks[i]);
            if(temp_flag) check_built_in = temp_flag;
        }
        
        if (strcmp(blocks[i], "<") == 0) {

            if (input_redirect_index || last_was_redirect) {
                //printf("< error, too many or close\n");
                return 0;
            }

            input_redirect_index = i;
            last_was_redirect = 1;
        }
        else if (strcmp(blocks[i], ">") == 0 || strcmp(blocks[i], ">>") == 0) {

            if (output_redirect_index || last_was_redirect) {
                //printf("> or >> error, too many or close\n");
                return 0;
            }

            output_redirect_index = i;
            last_was_redirect = 1;
        }
        else if (strcmp(blocks[i], "|") == 0) {

            if (output_redirect_index || last_was_redirect) {
                //printf("| error, close to another or > or >> happens before\n");
                return 0;
            }

            if(!first_pipe) first_pipe = i;
            pipe_index = i;

            last_was_redirect = 1;
        }
        else {
            last_was_redirect = 0;
        }
    }

    if(check_built_in) {
        if(output_redirect_index || input_redirect_index || pipe_index) {
            //printf("Built in cannot be redirect\n");
            return 0;
        }

        if(check_built_in != block_num) {
            //printf("Built in wrong parameters\n");
            return 0;
        }

        //printf("Built in command\n");
        return 2;
    }


    if (output_redirect_index + input_redirect_index != 0) {


        if (pipe_index > 0) {


            if (output_redirect_index && input_redirect_index) {       
                if (input_redirect_index > output_redirect_index || input_redirect_index != first_pipe - 2 || output_redirect_index != block_num - 2) {
                    //printf("Invalid < and > position [1]\n");
                    return 0;
                }
            }
            // 1个
            else {
                if (input_redirect_index != first_pipe - 2 && output_redirect_index != block_num - 2) {
                    //printf("Invalid < and > position [2]\n");
                    return 0;
               }
            }

        }

        else {

  
            if (output_redirect_index && input_redirect_index) {

                if (output_redirect_index > input_redirect_index) {
                    if (output_redirect_index != block_num - 2 || output_redirect_index - input_redirect_index != 2) {
                        //printf("Invalid < and > position [3]\n");
                        return 0;
                    }
                }
                else {
                    if (input_redirect_index != block_num - 2 || input_redirect_index - output_redirect_index != 2) {
                        //printf("Invalid < and > position [4]\n");
                        return 0;
                    }

                }

            }
            // 1个
            else {

                if (input_redirect_index != block_num - 2 && output_redirect_index != block_num - 2) {
                    //printf("Invalid < and > position [5]\n");
                    return 0;
                }
            }
        }
    }

    //printf("Common commands accepted\n");
   
    return 1;
}

void cmdDispatch() {


    // char command[1000];
    // fgets(command, sizeof(command), stdin);
    // command[strcspn(command, "\n")] = 0;

    char* command = NULL;
    size_t len_c = 0;         // Current length of the line
    ssize_t read_c;  

    read_c = getline(&command, &len_c, stdin);

    if( read_c==-1 || read_c==0) exit(0);

    if(read_c==1 && command[read_c-1]=='\n') return;

    if(command[read_c-1]=='\n') command[read_c-1] = '\0';
    

    int types = verifyCommand(command);

    // printf("types %d\n", types);

    if(types==0) {
        fprintf(stderr, "Error: invalid command\n");
    } else if(types==2) {

        execInCmd(command);
        // int type = execInCmd(command);
    } else if(types==1) {

        int result = handleInput(command);

        if(result==1) fprintf(stderr, "Error: invalid program\n");
        else if(result==2)fprintf(stderr, "Error: invalid file\n");
    }

    free(command);  // Release the allocated memory

}
