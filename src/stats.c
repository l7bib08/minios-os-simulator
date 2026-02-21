#include <stdio.h>
#include "stats.h"
#include "process.h"

static int total_ticks = 0;
static int context_switches = 0;

void stats_init(void) {
    total_ticks = 0;
    context_switches = 0;
}

void stats_on_tick(int tick_system, int prev_running_pid, int new_running_pid) {
    total_ticks = tick_system;

    if (new_running_pid != -1 && prev_running_pid != new_running_pid) {
        context_switches++;
    }
}

void stats_print(void) {
    int count = process_get_count();

    int terminated = 0;
    long sum_wt = 0;
    long sum_tat = 0;
    long sum_rt = 0;

    printf("=========================================\n");
    printf("                 STATS                   \n");
    printf("=========================================\n");
    printf("Total ticks: %d\n", total_ticks);
    printf("Total processes: %d\n", count);


    for (int i = 0; i < count; i++) {
        const Process *p = process_get_by_index(i);
        if (!p) continue;

        if (p->state != PROCESS_TERMINATED) continue;
        if (p->arrival_tick < 0 || p->finish_tick < 0) continue;

        int tat = p->finish_tick - p->arrival_tick;
        int wt = tat - p->burst_time;
        int rt = (p->first_run_tick >= 0) ? (p->first_run_tick - p->arrival_tick) : -1;

        terminated++;
        sum_tat += tat;
        sum_wt += wt;
        if (rt >= 0) sum_rt += rt;

        printf("%d\t%-10s\t%d\t%d\t%d\t%d\n",
               p->pid, p->name, wt, rt, tat, p->burst_time);
    }

    printf("\nTerminated: %d\n", terminated);

    double avg_wt = (terminated > 0) ? ((double)sum_wt / terminated) : 0.0;
    double avg_tat = (terminated > 0) ? ((double)sum_tat / terminated) : 0.0;
    double avg_rt = (terminated > 0) ? ((double)sum_rt / terminated) : 0.0;

    double throughput = 0.0;
    if (total_ticks > 0) throughput = (double)terminated / (double)total_ticks;

    printf("Average Waiting Time: %.2f\n", avg_wt);
    printf("Average Response Time: %.2f\n", avg_rt);
    printf("Average Turnaround Time: %.2f\n", avg_tat);
    printf("Throughput: %.4f proc/tick\n", throughput);
    printf("Context switches: %d\n", context_switches);
    printf("=========================================\n");
}