#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <pthread.h>
#include "./include/sender.h"
#include "./include/reciever.h"

int msgid;

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
