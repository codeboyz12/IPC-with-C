#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#define MAX 256

struct message_buff {
    long type;
    pid_t from_pid;
    pid_t to_pid;
    char cmd[10];
    char target[32];
    char text[MAX];
} msg;

struct dm {
    long type;
    pid_t from_pid;
    char message[MAX];
} response;

int main() {
    key_t key;
    int msgid;
    pid_t mypid;

    key = ftok("/tmp", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);
    msg.type = 1;

    mypid = getpid();
    printf("MY PID: %d\n", mypid);

    char line[512];

    while (1) {
        ssize_t received = msgrcv(msgid, &response, sizeof(response) - sizeof(long), mypid, 0);

        if (received == -1) {
            perror("msgrcv");
            break;
        }

        printf("[FROM: %d]: %s\n", response.from_pid, response.message);
        // printf("write data : ");
        // fgets(line, sizeof(line), stdin);
        // line[strcspn(line, "\n")] = '\0';

        // char *cmd = strtok(line, " \t");
        // char *target = strtok(NULL, " \t");
        // char *text = strtok(NULL, "");

        // if (!cmd) cmd = "";
        // if (!target) target = "";
        // if (!text) text = "";

        // if (strcmp(cmd, "AMI") == 0 || strcmp(cmd, "WHOAMI") == 0) {
        //     printf("I am process: %d\n", mypid);
        //     continue;
        // }

        // strncpy(msg.cmd, cmd, sizeof(msg.cmd) - 1);
        // strncpy(msg.target, target, sizeof(msg.target) - 1);
        // strncpy(msg.text, text, sizeof(msg.text) - 1);

        // msg.from_pid = mypid;

        // msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);

        // printf("Sent: CMD=%s | TARGET=%s | TEXT=%s\n",
        //        msg.cmd, msg.target, msg.text);
    }

    return 0;
}
