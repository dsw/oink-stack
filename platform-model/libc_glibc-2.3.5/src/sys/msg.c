#include <glibc-fs/prelude.h>
#include <sys/msg.h>

// struct msgbuf
// {
//     long int mtype;
//     char mtext[1];
// };

// msgqid_ds contains no char buffers on Linux
int msgctl (int msqid, int cmd, struct msqid_ds *buf) {}

int msgget (key_t key, int msgflg) {}

int msgrcv (int msqid, void *msgp, size_t msgsz, long int msgtyp, int msgflg)
{
    __DO_TAINT( ((struct msgbuf*) msgp)->mtext );
}

int msgsnd (int msqid, const void *msgp, size_t msgsz, int msgflg) {}
