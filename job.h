#ifndef JOB_H
#define JOB_H

typedef struct Job {
    int pid;              
    int job_id;     
    char *command;       
    struct Job *next;    
} Job;

Job* addJob(int pid, char *command);
void removeJob(int pid);
Job* findJob(int job_num);
void printJobs();
int hasSuspendedJobs();

#endif