#include <time.h>
#include "scheduler.h"
#include "process.h"

static int current_pid = -1;

void scheduler_init(void) {
    current_pid = -1;
}

void scheduler_fifo(void) {

    int running_pid = process_get_running_pid();
    if (running_pid != -1) {
        current_pid = running_pid;
        return;
    }

    int count = process_get_count();
    const Process *chosen = NULL;

    for (int i = 0; i < count; i++) {
        const Process *p = process_get_by_index(i);
        if (p == NULL) continue;

        if (p->state != PROCESS_READY) continue;

        if (chosen == NULL || p->start_time < chosen->start_time) {
            chosen = p;
        }
    }

    if (chosen == NULL) {
        current_pid = -1;
        return;
    }

    process_set_state_by_pid(chosen->pid, PROCESS_RUNNING);
    current_pid = chosen->pid;
}

void scheduler_rr(void) {
    
}



//Custom Event-Driven Preemptive Scheduler

//In this project, I designed a custom preemptive scheduling policy where each process declares its total CPU execution time at creation. The system tracks the remaining execution time of the running process and supports pausing and resuming execution by saving this remaining time.

//The scheduler starts by executing the first arriving process. A running process is preempted only under two specific conditions: when its total CPU time is fully consumed, or when a predefined number of new processes are created after it starts running. Upon preemption, the process state is preserved and another ready process is scheduled for execution.

//This scheduling policy is event-driven rather than time-quantum-based and is implemented alongside standard scheduling algorithms such as FIFO and Round Robin for comparison and experimentation. The design emphasizes process state management, preemption mechanisms, and scheduler extensibility rather than real-time fairness guarantees.