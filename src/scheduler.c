#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "scheduler.h"
#include "process.h"


static int rr_quantum;
static int running_pid;
static int start_tick;

static int *q;
static int capacity;
static int front_pid;
static int number_of_ready_pids;

static int current_pid = -1;

static void queue_init(int capacity_init);
static int  queue_is_empty(void);
static int  queue_pop_front_pid(void);
static void queue_push_back(int pid);

static void dispatch_next_rr(int tick_system) {

    while (!queue_is_empty()) {

        int next = queue_pop_front_pid();
        if (next <= 0) continue;

        ProcessState st;
        if (process_get_state_by_pid(next, &st) != 0) continue;
        if (st == PROCESS_TERMINATED) continue;

        process_set_state_by_pid(next, PROCESS_RUNNING);
        running_pid = next;
        start_tick = tick_system;
        return;
    }

    running_pid = -1;
}

void scheduler_fifo_init(void) {
    current_pid = -1;
}

void scheduler_fifo(void) {

    int already_running = process_get_running_pid();
    if (already_running != -1) {
        current_pid = already_running;
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


static void queue_init(int capacity_init) {

    if (capacity_init < 1) capacity_init = 1;

    free(q);
    q = NULL;

    q = (int*)malloc(capacity_init * sizeof(int));
    if (q == NULL) {
        printf("ERROR: queue allocation failed\n");
        capacity = 0;
        front_pid = 0;
        number_of_ready_pids = 0;
        return;
    }

    capacity = capacity_init;
    front_pid = 0;
    number_of_ready_pids = 0;
}

static int queue_is_empty(void) {
    return number_of_ready_pids == 0;
}

static int queue_pop_front_pid(void) {

    if (number_of_ready_pids == 0) return -1;

    int pid = q[front_pid];
    front_pid = (front_pid + 1) % capacity;
    number_of_ready_pids--;

    return pid;
}

static void queue_push_back(int pid) {

    if (pid <= 0) return;

    if (q == NULL || capacity <= 0) {
        capacity = 4;
        q = (int*)malloc(capacity * sizeof(int));
        if (q == NULL) {
            printf("ERROR: queue allocation failed\n");
            capacity = 0;
            front_pid = 0;
            number_of_ready_pids = 0;
            return;
        }
        front_pid = 0;
        number_of_ready_pids = 0;
    }

    if (number_of_ready_pids == capacity) {

        int old_capacity = capacity;
        int new_capacity = old_capacity * 2;

        int *new_q = (int*)malloc(new_capacity * sizeof(int));
        if (new_q == NULL) {
            printf("ERROR: queue grow failed\n");
            return;
        }

        for (int i = 0; i < number_of_ready_pids; i++) {
            new_q[i] = q[(front_pid + i) % old_capacity];
        }

        free(q);
        q = new_q;
        capacity = new_capacity;
        front_pid = 0;
    }

    int idx = (front_pid + number_of_ready_pids) % capacity;
    q[idx] = pid;
    number_of_ready_pids++;
}


void scheduler_rr_init(int quantum) {

    if (quantum < 1) quantum = 1;

    rr_quantum = quantum;
    running_pid = -1;
    start_tick = 0;

    queue_init(4);
}

void scheduler_rr_on_process_created(int pid) {
    if (pid < 1) return;
    queue_push_back(pid);
}

void scheduler_rr_step(int tick_system) {

    if (running_pid == -1) {
        dispatch_next_rr(tick_system);
        return;
    }

    ProcessState state;
    if (process_get_state_by_pid(running_pid, &state) != 0 ||
        state == PROCESS_TERMINATED) {

        running_pid = -1;
        dispatch_next_rr(tick_system);
        return;
    }

    int elapsed = tick_system - start_tick;
    if (elapsed < rr_quantum) {
        return;
    }

    process_set_state_by_pid(running_pid, PROCESS_READY);
    queue_push_back(running_pid);
    running_pid = -1;

    dispatch_next_rr(tick_system);
}


void scheduler_sjf_init(void) {
    current_pid = -1;
}

void scheduler_sjf_step(void) {

    int running = process_get_running_pid();

    if (running != -1) {
        int remaining = process_decrement_remaining_time(running);

        if (remaining < 0) return;

        if (remaining == 0) {
            process_set_state_by_pid(running, PROCESS_TERMINATED);
        }
        return;
    }

    int count = process_get_count();
    int best_pid = -1;
    int shortest = 999999;
    time_t best_start = 0;

    for (int i = 0; i < count; i++) {
        const Process *p = process_get_by_index(i);
        if (!p) continue;
        if (p->state != PROCESS_READY) continue;

        if (p->remaining_time < shortest) {
            shortest = p->remaining_time;
            best_pid = p->pid;
            best_start = p->start_time;
        } else if (p->remaining_time == shortest && best_pid != -1) {
            if (p->start_time < best_start ||
                (p->start_time == best_start && p->pid < best_pid)) {
                best_pid = p->pid;
                best_start = p->start_time;
            }
        } else if (p->remaining_time == shortest && best_pid == -1) {
            best_pid = p->pid;
            best_start = p->start_time;
        }
    }

    if (best_pid != -1) {
        process_set_state_by_pid(best_pid, PROCESS_RUNNING);
        current_pid = best_pid;
    } else {
        current_pid = -1;
    }
}