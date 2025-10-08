#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "./include/sender.h"
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
