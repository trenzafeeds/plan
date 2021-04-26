/******************************************
 * driver.c
 *
 * Unified driver program.
 * This functions as the top
 * level program for ALL implementations.
 * Uses a different definition of the node
 * function passed to each thread
 * according to implementation.
 ******************************************/

#include "driver.h"
#define TESTNODES 5
//#define MEMORY -- Should be defined by Makefile

pthread_barrier_t bar;

int main(int argc, char *argv[])
{
  int nodes;
  srandom(time(0));

  if (argc <= 1)
    nodes = TESTNODES;
  else {
    nodes = atoi(argv[1]);
    if (nodes > MAXNODES) {
      printf("Error: MAXNODES is %d. This is restricted by POSIX message queue limits and cannot be changed.\n",\
             MAXNODES);
      exit(1);
    } else if (nodes < 5) {
      printf("Error: Sorry, Paxos only likes to be run with 5 or more nodes.\n");
      exit(1);
    }
  }
  debug("Starting with %d nodes.\n", nodes);

  #ifdef MEMORY
  shared_memory shared_mem = scalloc(nodes, B_SIZE);
  #endif

  struct args *targs = scalloc(nodes, ARGSIZE);
  void *pass[nodes];
  pthread_t *thread_array = new_thread_array(nodes);
  pthread_barrier_init(&bar, NULL, nodes);
  for (int i = 0; i < nodes; i++) {
    targs[i].id = FIRSTID + i;
    targs[i].nodecount = nodes;
    #ifdef MEMORY
    targs[i].extra = (void *) shared_mem;
    #endif
    pass[i] = &targs[i];
  }
  create_threads(thread_array, nodes, NULL, &node, pass);
  sleep(1);
  pthread_cond_broadcast(&setup);
  join_threads(thread_array, nodes, NULL);
  free(targs);
  free(thread_array);
  return 0;
}
