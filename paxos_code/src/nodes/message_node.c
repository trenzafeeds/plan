/*********************************
 * message_node.c
 *
 * Routine run by each thread
 * in the distributed
 * message-passing AND parallel
 * message-passing (ring)
 * implementations.
 *********************************/

#include "node.h"
#include "messages.h"

pthread_barrier_t bar;

void node(struct args *argstruct)
{
  int ret;
  proc_info myinfo = scalloc(1, PROC_INFO_SIZE);
  myinfo->pid = argstruct->id;
  myinfo->M = argstruct->nodecount;
  myinfo->timeout = 0;
  char *myq = nid(argstruct->id);
  myinfo->listen = init_queue(myq, O_NONBLOCK, MAXMSG, M_SIZE);
  int cleared = clear_queue(myinfo->listen);

  debug("Thread %d starting wait.\n", myinfo->pid);
  pthread_barrier_wait(&bar);
  debug("Thread %d back from wait.\n", myinfo->pid);
  
  while ((ret = paxos(myinfo)) == -1)
    sleep(1);

  debug("Thread %d decided on %d as leader.\n", myinfo->pid, ret);
  close_queue(myinfo->listen);
  mq_unlink(myq);
  free(myinfo);
  return; 
}
