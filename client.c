#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX 256

struct message_buff {
    long type;
    pid_t from_pid;
    pid_t to_pid;
    char cmd[10];
    char text[256];
} msg;

struct dm {
    long type;
    pid_t from_pid;
    char message[MAX];
} response;

int msgid;
char line[512];

void* receiver(void* arg) {
    while (1)
    {
        if (msgrcv(msgid, &response, sizeof(response) - sizeof(long), getpid(), 0) != -1){
            printf("\n[FROM %d]: %s\n", response.from_pid, response.message);
            printf("IAM [%d]: ", getpid());
            fflush(stdout);
        }
    }
    return NULL;
}

void* sender(void* arg) {
    while (1) {
        printf("IAM [%d]: ", getpid());
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            break;

        line[strcspn(line, "\n")] = '\0'; // remove newline

        // --- parse ---
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

        // --- fill message struct ---
        msg.from_pid = getpid();
        msg.to_pid = target_pid;
        strncpy(msg.cmd, cmd, sizeof(msg.cmd) - 1);
        msg.cmd[sizeof(msg.cmd) - 1] = '\0';
        strncpy(msg.text, text, sizeof(msg.text) - 1);
        msg.text[sizeof(msg.text) - 1] = '\0';

        // --- choose routing type ---
        if (strcmp(cmd, "DM") == 0) {
            msg.type = 1;  // send to main process for routing
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);

            // printf("Sent DM: from %d → to %d : %s\n",
            //        msg.from_pid, msg.to_pid, msg.text);
        } else if (strcmp(cmd, "BROAD") == 0) {
            msg.type = 1;
            msg.to_pid = 0; // broadcast
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
            printf("Broadcast: %s\n", msg.text);
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }
    return NULL;
}

int main() {
    key_t key = ftok("/tmp", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);

    pthread_t t_reciever;
    pthread_t t_sender;

    pthread_create(&t_reciever, NULL, receiver, NULL);
    pthread_create(&t_sender, NULL, sender, NULL);

    pthread_join(t_reciever, NULL);
    pthread_join(t_sender, NULL);

    return 0;
}
