#ifndef PROCESS_H
#define PROCESS_H

#include <time.h> 


typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_TERMINATED
} ProcessState;


typedef struct {
    int pid;                
    char name[20];          
    ProcessState state;     
    time_t start_time;      
} Process;


void process_init(void);

void process_create(const char *name);

int process_find_by_name(const char *name, int *pid_array, int max_result);

void process_kill(int pid);

void process_list(void);


#endif 
