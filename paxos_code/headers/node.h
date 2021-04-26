/*****************************
 * node.h
 * 
 * Unified header file for
 * all "node" functions, the
 * routine run by each thread.
 * Defined differently for
 * each implementation.
 *****************************/

#include "paxos.h"

#ifndef NODEH
#define NODEH

/* Struct allowing argument
 * data to be passed to the thread
 * by the driver program. The 
 * void *extra is used to point
 * to shared memory when applicable.
 */
struct args {
  int id;
  int nodecount;
  void *extra;
};
#define ARGSIZE sizeof(struct args)

pthread_cond_t setup;
pthread_mutex_t lock;

void node(struct args *argstruct);

#endif /* NODEH */
