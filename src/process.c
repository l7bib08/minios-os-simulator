#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "process.h"

static Process *table = NULL;
static int process_count = 0;
static int next_pid = 1;

static const char *state_to_string(ProcessState s) {
    switch (s) {
        case PROCESS_READY:      return "READY";
        case PROCESS_RUNNING:    return "RUNNING";
        case PROCESS_TERMINATED: return "TERMINATED";
        default:                 return "UNKNOWN";
    }
}

void process_init(void) {
    free(table);
    table = NULL;
    process_count = 0;
    next_pid = 1;
}

void process_create(const char *name) {
    if (name == NULL || name[0] == '\0') {
        printf("Error: invalid process name.\n");
        return;
    }

    Process *tmp = realloc(table, (process_count + 1) * sizeof(Process));
    if (tmp == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return;
    }

    table = tmp;

    Process *p = &table[process_count];
    p->pid = next_pid++;
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';
    p->state = PROCESS_READY;
    p->start_time = time(NULL);

    process_count++;

    printf("Process created: PID=%d, NAME=%s\n", p->pid, p->name);
}

int process_create_return_pid(const char *name) {
    int before = next_pid;
    process_create(name);
    if (next_pid == before) return -1; 
    return before;
}

void process_create_with_burst(const char *name, int burst) {
    if (name == NULL || name[0] == '\0') {
        printf("Error: invalid process name.\n");
        return;
    }
    if (burst <= 0) {
        printf("Error: invalid burst.\n");
        return;
    }

    Process *tmp = realloc(table, (process_count + 1) * sizeof(Process));
    if (tmp == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return;
    }

    table = tmp;
    Process *p = &table[process_count];

    p->pid = next_pid++;
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';
    p->state = PROCESS_READY;
    p->start_time = time(NULL);

    p->burst_time = burst;
    p->remaining_time = burst;

    process_count++;

    printf("Process created: PID=%d, NAME=%s, BURST=%d\n", p->pid, p->name, burst);
}

int process_create_with_burst_return_pid(const char *name, int burst) {
    int before = next_pid;
    process_create_with_burst(name, burst);
    if (next_pid == before) return -1;
    return before;
}

int process_find_by_name(const char *name, int *pid_array, int max_result) {
    if (name == NULL || name[0] == '\0' || pid_array == NULL || max_result <= 0)
        return -1;

    int found = 0;

    for (int i = 0; i < process_count; i++) {
        if (strcmp(table[i].name, name) == 0) {
            if (found == max_result) break;
            pid_array[found] = table[i].pid;
            found++;
        }
    }

    return found;
}

void process_kill_by_pid(int pid) {
    if (pid <= 0) {
        printf("Error: invalid PID.\n");
        return;
    }

    for (int i = 0; i < process_count; i++) {
        if (table[i].pid == pid) {
            if (table[i].state == PROCESS_TERMINATED) {
                printf("Process PID=%d is already terminated.\n", pid);
                return;
            }
            table[i].state = PROCESS_TERMINATED;
            printf("Process terminated successfully: PID=%d\n", pid);
            return;
        }
    }

    printf("No process with PID=%d has been found.\n", pid);
}

void process_list(void) {
    if (process_count == 0) {
        printf("No processes.\n");
        return;
    }

    printf("PID\tNAME\t\tSTATE\t\tSTART_TIME\tBURST\tREMAIN\n");
    printf("-----------------------------------------------------------------\n");

    for (int i = 0; i < process_count; i++) {

        printf("%d\t%-10s\t%-10s\t%ld\t%d\t%d\n",
            table[i].pid,
            table[i].name,
            state_to_string(table[i].state),
            (long)table[i].start_time,
            table[i].burst_time,
            table[i].remaining_time);

    }
}

void process_list_same_name(const char *name) {
    if (name == NULL || name[0] == '\0') {
        printf("Error: invalid name.\n");
        return;
    }

    int printed = 0;

    printf("PID\tNAME\t\tSTATE\t\tSTART_TIME\n");
    printf("-------------------------------------------------\n");

    for (int i = 0; i < process_count; i++) {
        if (strcmp(table[i].name, name) == 0) {
            printf("%d\t%-10s\t%-10s\t%ld\n",
                   table[i].pid,
                   table[i].name,
                   state_to_string(table[i].state),
                   (long)table[i].start_time);
            printed++;
        }
    }

    if (printed == 0) {
        printf("No process named '%s' has been found.\n", name);
    }
}

void process_kill_by_name(const char *name) {
    if (name == NULL || name[0] == '\0') {
        printf("Error: invalid name.\n");
        return;
    }

    int pids[100];
    int count = process_find_by_name(name, pids, 100);

    if (count == -1) {
        printf("Error: invalid search parameters.\n");
        return;
    }

    if (count == 0) {
        printf("No process named '%s' has been found.\n", name);
        return;
    }

    if (count == 1) {
        process_kill_by_pid(pids[0]);
        return;
    }

    printf("Multiple processes named '%s' were found.\n", name);
    printf("Please choose a PID to kill from the list below:\n");
    process_list_same_name(name);
}


int process_get_count(void) {
    return process_count;
}

const Process* process_get_by_index(int index) {
    if (index < 0 || index >= process_count) return NULL;
    return &table[index];
}

int process_get_state_by_pid(int pid, ProcessState *out_state) {
    if (pid <= 0 || out_state == NULL) return -1;

    for (int i = 0; i < process_count; i++) {
        if (table[i].pid == pid) {
            *out_state = table[i].state;
            return 0;
        }
    }
    return -1;
}

int process_set_state_by_pid(int pid, ProcessState new_state) {
    if (pid <= 0) return -1;

    for (int i = 0; i < process_count; i++) {
        if (table[i].pid == pid) {
            table[i].state = new_state;
            return 0;
        }
    }
    return -1;
}

int process_get_running_pid(void) {
    for (int i = 0; i < process_count; i++) {
        if (table[i].state == PROCESS_RUNNING) {
            return table[i].pid;
        }
    }
    return -1;
}


int *process_get_pids(int count) {
    if (count <= 0) return NULL;

    int *list_of_pids = malloc(count * sizeof(int));
    if (!list_of_pids) return NULL;

    for (int i = 0; i < count && i < process_count; i++) {
        list_of_pids[i] = table[i].pid;
    }

    return list_of_pids;
}