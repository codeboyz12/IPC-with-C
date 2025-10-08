#include "../include/reciever.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
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