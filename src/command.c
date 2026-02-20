#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "command.h"
#include "process.h"
#include "scheduler.h"

static int tick_system = 0;
static int scheduler_mode = 3;

void set_scheduler_mode(int mode) {
    scheduler_mode = mode;
}

static void remove_newline(char *s) {
    if (s == NULL) return;
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[n - 1] = '\0';
        n--;
    }
}

static int is_all_digits(const char *s) {
    if (!s || !*s) return 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

void check_input(const char *input) {
    if (input == NULL || input[0] == '\0') return;

    char cmnd[32] = {0};
    char rest[256] = {0};

    int n = sscanf(input, "%31s %255[^\n]", cmnd, rest);
    if (n < 1) return;

    remove_newline(rest);

    if (strcmp(cmnd, "help") == 0) {
        puts("Commands: run <name> <burst>, kill <pid|name>, ps, help, exit");
        tick_system++;
    } else if (strcmp(cmnd, "exit") == 0) {
        exit(0);
    } else if (strcmp(cmnd, "run") == 0) {
        if (n < 2 || rest[0] == '\0') {
            printf("Usage: run <name> <burst>\n");
            tick_system++;
        } else {
            char name[20] = {0};
            int burst = 0;

            int m = sscanf(rest, "%19s %d", name, &burst);
            if (m != 2 || burst <= 0) {
                printf("Usage: run <name> <burst>\n");
                tick_system++;
            } else {
                int pid = process_create_with_burst_return_pid(name, burst);
                tick_system++;
                if (pid > 0 && scheduler_mode == 2) {
                    scheduler_rr_on_process_created(pid);
                }
            }
        }
    } else if (strcmp(cmnd, "kill") == 0) {
        if (n < 2 || rest[0] == '\0') {
            printf("invalid argument\n");
            tick_system++;
        } else {
            tick_system++;
            if (is_all_digits(rest)) {
                int pid = atoi(rest);
                if (pid <= 0) printf("invalid PID\n");
                else process_kill_by_pid(pid);
            } else {
                process_kill_by_name(rest);
            }
        }
    } else if (strcmp(cmnd, "ps") == 0) {
        process_list();
        tick_system++;
    } else {
        printf("Unknown command: %s\n", cmnd);
        tick_system++;
    }

    if (scheduler_mode == 1) {
        scheduler_fifo();
    } else if (scheduler_mode == 2) {
        scheduler_rr_step(tick_system);
    } else {
        scheduler_sjf_step();
    }
}