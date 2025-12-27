#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "process.h"

static Process *table = NULL;
static int process_count = 0;
static int next_pid = 1;

void process_init(void) {
    free(table);
    table = NULL;
    process_count = 0;
    next_pid = 1;
}



void process_create(const char *name) {

    Process *tmp = realloc(table, (process_count + 1) * sizeof(Process));
    if (tmp == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        return;
    }

    table = tmp;

    Process *p = &table[process_count];

    p->pid = next_pid;

    next_pid++;

    strncpy(p->name, name, sizeof(p->name) - 1);

    p->name[sizeof(p->name) - 1] = '\0';

    p->state = PROCESS_READY;

    p->start_time = time(NULL);
    
    process_count++;

    printf("Process created: PID=%d, NAME=%s\n", p->pid, p->name);
}


int process_find_by_name(const char *name, int *pid_array, int max_result) {

    if (name == NULL || name[0] == '\0' || pid_array == NULL || max_result <= 0) return -1;
    int found = 0;
    for (int i = 0; i < process_count; i++) {
        if (strcmp(table[i].name, name) == 0 ) {
            if (found == max_result) break;
            else {
                pid_array[found] = table[i].pid;
                found++;
            }
        }
    }
return found;
}


void process_kill(int pid) {
    if (pid <= 0) { 
        printf("PID invalide !");
        return;
    }
    else { for (int i = 0; i < process_count; i++) {
                if (table[i].pid == pid && table[i].state == PROCESS_TERMINATED) {
                    printf("Already terminated !");
                    return;
                }
                else {
                    if (pid == table[i].pid) {
                        table[i].state = PROCESS_TERMINATED;
                        printf("Process terminated succesfully");
                        return;
                    }
                }
            }
        printf("No process with this id has been found");
    }
}


void process_list(void) {
    if (process_count == 0) {
        printf("No processes.\n");
        return;
    }

    printf("PID\tNAME\t\tSTATE\t\tSTART_TIME\n");
    printf("-------------------------------------------------\n");

    for (int i = 0; i < process_count; i++) {
        const char *state_str;

        switch (table[i].state) {
            case PROCESS_READY:
                state_str = "READY";
                break;
            case PROCESS_RUNNING:
                state_str = "RUNNING";
                break;
            case PROCESS_TERMINATED:
                state_str = "TERMINATED";
                break;
            default:
                state_str = "UNKNOWN";
        }

        printf("%d\t%-10s\t%-10s\t%ld\n",
               table[i].pid,
               table[i].name,
               state_str,
               table[i].start_time);
    }
}
