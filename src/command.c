#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "command.h"
#include "process.h"

static void trim_newline(char *s) {
    if (s == NULL) return;
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[n - 1] = '\0';
        n--;
    }
}

void check_input(const char *input) {

    if (input == NULL || input[0] == '\0') {
        return;
    }

    char cmnd[32] = {0};
    char rest[256] = {0};
    int isnumber = 0;
    int isletter = 0;

    int n = sscanf(input, "%31s %255[^\n]", cmnd, rest);
    if (n < 1) return;

    trim_newline(rest);

    if (strcmp(cmnd, "help") == 0) {
        puts("Commands: run <name>, kill <pid|name>, ps, help, exit");
        return;
    }

    if (strcmp(cmnd, "exit") == 0) {
        exit(0);
    }

    if (strcmp(cmnd, "run") == 0) {
        if (n < 2 || rest[0] == '\0') {
            printf("name invalid\n");
            return;
        }
        process_create(rest);
        return;
    }

    if (strcmp(cmnd, "kill") == 0) {
        if (n < 2 || rest[0] == '\0') {
            printf("invalid argument\n");
            return;
        }

        for (int i = 0; rest[i] != '\0'; i++) {
            unsigned char c = (unsigned char)rest[i];
            if (isdigit(c)) isnumber = 1;
            else if (isalpha(c)) isletter = 1;
        }

        if (isnumber && isletter) {
            printf("invalid argument\n");
            return;
        }

        if (isletter) {
            process_kill_by_name(rest);
            return;
        }

        if (isnumber) {
            int pid = atoi(rest);
            if (pid <= 0) {
                printf("invalid PID\n");
                return;
            }
            process_kill_by_pid(pid);
            return;
        }

        printf("invalid argument\n");
        return;
    }

    if (strcmp(cmnd, "ps") == 0) {
        process_list();
        return;
    }

    printf("Unknown command: %s\n", cmnd);
}