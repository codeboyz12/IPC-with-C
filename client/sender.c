#include "../include/sender.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct message_buff {
    long type;
    pid_t from_pid;
    pid_t to_pid;
    char cmd[10];
    char text[256];
} msg;

int msgid;
char line[512];

void* sender(void* arg) {
    while (1) {
        printf("IAM [%d]: ", getpid());
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            break;

        line[strcspn(line, "\n")] = '\0';

        char *cmd = strtok(line, " \t");
        char *target_str = strtok(NULL, " \t");
        char *text = strtok(NULL, "");

        if (!cmd) cmd = "";
        if (!target_str) target_str = "";
        if (!text) text = "";

        pid_t target_pid = 0;
        if (strlen(target_str) > 0) {
            char *end;
            long val = strtol(target_str, &end, 10);
            if (*end == '\0') target_pid = (pid_t)val;
        }

        msg.from_pid = getpid();
        msg.to_pid = target_pid;
        strncpy(msg.cmd, cmd, sizeof(msg.cmd) - 1);
        msg.cmd[sizeof(msg.cmd) - 1] = '\0';
        strncpy(msg.text, text, sizeof(msg.text) - 1);
        msg.text[sizeof(msg.text) - 1] = '\0';

        if (strcmp(cmd, "DM") == 0) {
            msg.type = 1;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);

        } else if (strcmp(cmd, "BROAD") == 0) {
            msg.type = 1;
            msg.to_pid = 0;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
            printf("Broadcast: %s\n", msg.text);
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }
    return NULL;
}