/*******************************
 * memory_node.c
 *
 * Routine run by each
 * thread in the parallel
 * shared memory implementation.
 *******************************/

#include "node.h"
#include "memory.h"

pthread_barrier_t bar;

void node(struct args *argstruct)
{
  int ret;
  proc_info myinfo = scalloc(1, PROC_INFO_SIZE);
  myinfo->pid = argstruct->id;
  myinfo->M = argstruct->nodecount;
  myinfo->highest_seen = 1;
  myinfo->smem = (shared_memory) argstruct->extra;

  debug("Thread %d starting wait.\n", myinfo->pid);
  pthread_barrier_wait(&bar);
  debug("Thread %d back from wait.\n", myinfo->pid);

  while ((ret = paxos(myinfo)) == -1)
    sleep(1);

  debug("Thread %d decided on %d as leader.\n", myinfo->pid, ret);
  free(myinfo);
  return;
}
