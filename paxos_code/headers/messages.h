/****************
 * messages.h
 ****************/

#include "system.h"

/* POSIX Messages Libraries */
#include <mqueue.h>
#include <fcntl.h>

#ifndef MESSAGESHEADER
#define MESSAGESHEADER

#define TIMEOUT 2

#define DESCSIZE 4
#define DESCPREF "/_"

#define MAXMSG 10

typedef struct _message *message;
struct _message {
  int type;
  int id;
  int val;
  int auth;
};
#define M_SIZE sizeof(struct _message)

/* Individual threads must store different
 * local information depending on whether
 * they communicate using messages or shared
 * memory. The struct _proc_info below is defined
 * once here, and another time (differently) in
 * memory.h, with the correct definition included
 * as needed.
 */
struct _proc_info {
  int pid;
  int round;
  int role;
  int M;

  /* USED FOR EVERYTHING */
  int promised_id;

  /* proposer */
  int highest_old_id;
  int old_id_val;

  int proposed_val;
  int count;

  /* acceptor */
  int accepted_id;
  int accepted_val;

  /* leader */
  int leader_count;

  int timeout;
  mqd_t listen;
  message new_m;
};
#define PROC_INFO_SIZE sizeof(struct _proc_info)

message new_message(int type, int id, int value, int auth);
int send_m(message m_content, int *dests, int N);
int send_all(message m_content, int pid, int firstid, int N);
message receive_m(mqd_t mqdes);

char *nid(int id);
mqd_t init_queue(char *desc, long flags, long m_max, long m_size);
mqd_t open_queue(char *desc, int permissions);
int clear_queue(int id);
int close_queue(mqd_t mq_des);

#endif /* MESSAGESHEADER */

