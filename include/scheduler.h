#ifndef SCHEDULER_H
#define SCHEDULER_H

void scheduler_fifo_init(void);
void scheduler_fifo(void);

void scheduler_rr_init(int quantum);
void scheduler_rr_on_process_created(int pid);
void scheduler_rr_step(int tick_system);

void scheduler_sjf_init(void);
void scheduler_sjf_step(void);

#endif