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
    int burst_time;
    int remaining_time;
} Process;

void process_init(void);

void process_create(const char *name);
int  process_create_return_pid(const char *name);

void process_create_with_burst(const char *name, int burst);
int  process_create_with_burst_return_pid(const char *name, int burst);

int  process_find_by_name(const char *name, int *pid_array, int max_result);

void process_kill_by_pid(int pid);
void process_kill_by_name(const char *name);

void process_list(void);
void process_list_same_name(const char *name);

int process_get_count(void);
const Process* process_get_by_index(int index);

int process_get_state_by_pid(int pid, ProcessState *out_state);
int process_set_state_by_pid(int pid, ProcessState new_state);
int process_get_running_pid(void);

int process_decrement_remaining_time(int pid);

int* process_get_pids(int count);

#endif