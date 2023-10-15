#ifndef HELP_H
#define HELP_H

// command
void cmdDispatch();
int verifyCommand(char* command_0);

// dealDirCmd

int execInCmd(char* input);
void execDir(char** cmd, char* input, char* output, int inType, int outType, int p_fd[][2], int index, int pipes);

// prompt
void prompt();

// built in
void myExit();
void myCd(char *arg[]);
void myJobs();
void myFg(char *arg[]);



#endif // HELP_H