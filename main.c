#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>

#define MAX 256

// โครงสร้างข้อความที่ client จะส่งมา
struct message_buff {
    long type;
    pid_t from_pid;
    pid_t to_pid;
    char cmd[10];
    char text[MAX];
};

struct dm {
    long type;
    pid_t from_pid;
    char message[MAX];
};

int main() {
    key_t key;
    int msgid;
    struct message_buff msg;

    // สร้างหรือเปิดคิวข้อความ
    key = ftok("/tmp", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("[Main] Listening for messages...\n");

    while (1) {
        ssize_t received = msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0);
        if (received == -1) {
            perror("msgrcv");
            break;
        }

        printf("[From PID %d] %s %d %s\n", msg.from_pid, msg.cmd, msg.to_pid, msg.text);
        fflush(stdout);

        if( msg.to_pid != 0 ){
            struct dm message_dm;

            message_dm.type = msg.to_pid;
            message_dm.from_pid = msg.from_pid;
            strncpy(message_dm.message, msg.text, sizeof(message_dm.message) - 1);

            msgsnd(msgid, &message_dm, sizeof(message_dm) - sizeof(long), 0);
        }   
    }

    // ลบคิวเมื่อเลิกใช้งาน (ถ้าต้องการ)
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
