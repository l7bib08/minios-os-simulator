#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"
#include "scheduler.h"
#include "command.h"

static void flush_stdin_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static int read_int_line(const char *prompt, int default_value) {
    char buf[64];
    printf("%s", prompt);
    fflush(stdout);

    if (!fgets(buf, sizeof(buf), stdin)) return default_value;

    char *end = NULL;
    long v = strtol(buf, &end, 10);
    if (end == buf) return default_value;

    while (*end == ' ' || *end == '\t') end++;
    if (*end != '\0' && *end != '\n' && *end != '\r') return default_value;

    return (int)v;
}

static void print_header(void) {
    printf("=========================================\n");
    printf("            MiniOS v1 Simulator          \n");
    printf("=========================================\n");
}

static void print_commands(void) {
    printf("\nAvailable Commands:\n");
    printf("  run <name> <burst>\n");
    printf("  kill <pid|name>\n");
    printf("  ps\n");
    printf("  help\n");
    printf("  exit\n");
    printf("\n");
}

static void print_scheduler_menu(void) {
    printf("\nScheduler Selection:\n");
    printf("  [1] FIFO\n");
    printf("  [2] Round Robin\n");
    printf("  [3] SJF\n");
}

int main(void) {
    process_init();

    print_header();
    print_commands();
    print_scheduler_menu();

    int choice = read_int_line("Enter choice (default 3): ", 3);

    int quantum = 2;

    if (choice == 1) {
        scheduler_fifo_init();
        set_scheduler_mode(1);
        printf("\nSelected: FIFO\n");
    } else if (choice == 2) {
        quantum = read_int_line("Enter quantum (default 2): ", 2);
        if (quantum <= 0) quantum = 2;
        scheduler_rr_init(quantum);
        set_scheduler_mode(2);
        printf("\nSelected: Round Robin (q=%d)\n", quantum);
    } else {
        scheduler_sjf_init();
        set_scheduler_mode(3);
        printf("\nSelected: SJF\n");
    }

    printf("\n=========================================\n");
    printf("System Ready. Type commands below.\n");
    printf("=========================================\n\n");

    char input[512];

    while (1) {
        printf("MiniOS> ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n");
            break;
        }

        check_input(input);
    }

    return 0;
}