/***************************
 * messages.c
 *
 * Functions managing
 * message-passing between
 * threads using POSIX
 * message queues.
 ***************************/

#include "messages.h"

message new_message(int type, int id, int value, int auth)
{
  message new_m = smalloc(M_SIZE);
  new_m->type = type;
  new_m->id = id;
  new_m->val = value;
  new_m->auth = auth;
  return new_m;
}

int send_m(message m_content, int *dests, int N)
{
  int ret = 0;
  int prio;
  if ((m_content->type == 4) || (m_content->type==5)) prio = 1;
  else prio = 0;
  for (int i = 0; i < N; i++) {
    char *path = nid(dests[i]);
    mqd_t target;
    if ((target = open_queue(path, O_WRONLY | O_NONBLOCK)) != -1) {
      if ((ret = mq_send(target, (char *) m_content, M_SIZE, prio)) == -1) {
	debug("Message send failed.\n");
      }
      close_queue(target);
    } else {
      ret = ret - 1;
    }
    free(path);
  }
  free(m_content);
  return ret;
}

int send_all(message m_content, int pid, int firstid, int N)
{
  int destinations[N - 1];
  int j = 0;
  for (int i = firstid; i < firstid + N; i++) {
    if (i != pid) {
      destinations[j] = i;
      j++;
    }
  }
  return send_m(m_content, destinations, N - 1);
}

message receive_m(mqd_t mqdes)
{
  message m_content = smalloc(M_SIZE);
  if (mq_receive(mqdes, (char *) m_content, M_SIZE, NULL) == -1) {
    free(m_content);
    return -1;
  }
  return m_content;
}

/* Threads passing messages refer to eachother by an
 * integer id number only. The function below
 * converts id numbers into strings representing
 * POSIX message queues.
 */
char *nid(int id)
{
  char *desc = smalloc(sizeof(char) * DESCSIZE);
  char c = id;
  snprintf(desc, DESCSIZE, "%s%c", DESCPREF, c);
  return desc;
}

mqd_t init_queue(char *desc, long flags, long m_max, long m_size)
{
  mqd_t mq_des;
  mode_t mode = 0664;
  struct mq_attr attr;
  attr.mq_maxmsg = m_max;
  attr.mq_msgsize = m_size;
  attr.mq_flags = 0;
  if ((mq_des = mq_open(desc, flags | O_CREAT | O_RDWR, mode, &attr)) == -1)
    sys_error("Error at mq_open", 1);
  return mq_des;
}

mqd_t open_queue(char *desc, int permissions)
{
  mqd_t mq_des;
  if ((mq_des = mq_open(desc, permissions)) == -1)
    return -1;
  return mq_des;
}

int clear_queue(mqd_t des)
{
  message delete;
  int count = 0;
  while ((delete = receive_m(des)) != -1) {
    free(delete);
    count++;
  }
  return count;
}

int close_queue(mqd_t mq_des)
{
  int retval;
  if ((retval = mq_close(mq_des)) == -1)
    sys_error("Error at mq_close", 1);
  return retval; 
}
