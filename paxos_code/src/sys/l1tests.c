/*********************************
 * l1tests.c
 * 
 * A series of tests for
 * functions managing threads,
 * messaging, and shared memory.
 *********************************/

#include "l1tests.h"

int memtest(int nodes)
{
  int tval = 5;
  int tid = 1;
  int tblockid = 1;
  int ret;
  
  printf("Starting L1 shared memory test...\n");
  shared_memory smem = new_shared_mem(nodes);
  debug("Created shared block for %i processes.\n", nodes);
  
  struct block *wblock = new_block();
  wblock->val = tval;
  wblock->id = tid;
  ret = write_block(wblock, tblockid, smem);
  debug("write_block returned %i.\n", ret);

  ret = wipe_block(wblock);
  debug("wipe_block returned %i.\n", ret);

  ret = read_block(wblock, tblockid, smem);
  debug("read_block returned %i.\n", ret);

  if ((wblock->val == tval) && (wblock->id == tid)) {
    printf("L1 shared memory test successful.\n");
    ret = 0;
  } else {
    printf("L1 shared memory test failed, check debug log for details.\n");
    ret = 1;
  }

  free(smem);
  free(wblock);
  return ret;
}

int messagetest()
{
  int qid = 101;
  long qflags = O_CREAT | O_RDWR | O_NONBLOCK;
  int tval = 5;
  int tid = 1;
  int ret;

  printf("Starting L1 message passing test...\n");
  mqd_t mqueue = init_queue(nid(qid), qflags, MAXMSG, M_SIZE);
  debug("Initiated test message queue %s with id %i.\n", nid(qid), qid);

  message tmessage = new_message(0, tid, tval, 0);
  ret = send_m(tmessage, &qid, 1);
  debug("send_m returned %i.\n", ret);

  message rmessage = receive_m(mqueue);
  if (rmessage == -1)
    debug("recieve_m failed.");
  else {
    debug("recieve_m returned successfully.\n");
    if ((rmessage->val == tval) && (rmessage->id == tid)) {
      printf("L1 message passing test successful.\n");
      ret = 0;
    } else {
      printf("L1 message passing test failed, check debug log for details.\n");
      ret = 1;
    }
  }

  free(tmessage);
  mq_unlink(nid(qid));
  return ret;
}

void threadfunc(void *id)
{
  fflush(stdout);
  fprintf(stdout, "Thread %d printing.\n", *(int *)id);
  fflush(stdout);
}

int threadtest(int nodes)
{
  int ret;
  int *targs = scalloc(nodes, sizeof(int));
  int *pass[nodes];
  for (int i = 0; i < nodes; i++) {
    targs[i] = i;
    pass[i] = &targs[i];
  }

  printf("Starting L1 thread test...\n");
  pthread_t *threads = new_thread_array(nodes);
  debug("Allocated pthread array for %i threads.\n", nodes);

  create_threads(threads, nodes, NULL, &threadfunc, pass);
  join_threads(threads, nodes, NULL);
  debug("Successfully joined %i threads.\n", nodes);
  free(targs);
  free(threads);
  return 0;
}

void thread_memfunc(void *arg)
{
  struct mem_arg *cargs = arg;
  struct block *my_block = new_block();
  my_block->val = cargs->id;
  my_block->id = cargs->id;
  write_block(my_block, cargs->id, cargs->smem);
  free(my_block);
  return;
}

int thread_memtest(int nodes)
{
  int ret = 0;
  shared_memory shared = new_shared_mem(nodes);
  struct mem_arg *targs = scalloc(nodes, MEM_ARG_SIZE);
  void *pass[nodes];
  for (int i = 0; i < nodes; i++) {
    (&targs[i])->id = i;
    (&targs[i])->smem = shared;
    pass[i] = &targs[i];
  }
  
  printf("Starting L1 thread shared memory test...\n"); 
  pthread_t *threads = new_thread_array(nodes);

  create_threads(threads, nodes, NULL, &thread_memfunc, pass);
  join_threads(threads, nodes, NULL);
  
  for (int i = 0; i < nodes; i++) {
    if ((&shared[i])->val != i) ret = 1;
    debug("Value at block %d: %d\n", i, (&shared[i])->val);
  }
  free(shared);
  free(targs);
  free(threads);
  return ret;
}

void thread_msgfunc(void *id)
{
  int tval = 5;
  int tid = 1;

  if (*(int *)id == 0) {
    message testm = new_message(0, tid, tval, 0);
    int mqid = 101;
    int *dest = &mqid;
    send_m(testm, dest, 1);
    debug("Sent message\n");
    return;
  } else if (*(int *)id == 1) {
    mqd_t listen = open_queue(nid(101), O_RDONLY);
    message rec;
    while ((rec = receive_m(listen)) == -1) {
      sleep(1);
    }
    printf("Message id: %d\n", rec->id);
    printf("Message val: %d\n", rec->val);
    free(rec);
    return;
  }
}

int thread_msgtest()
{
  int qid = 101;
  long qflags = O_CREAT | O_RDWR | O_NONBLOCK;
  mqd_t mqueue = init_queue(nid(qid), qflags, MAXMSG, M_SIZE);
  int *targs = scalloc(2, sizeof(int));
  int *pass[2];
  for (int i = 0; i < 2; i++) {
    targs[i] = i;
    pass[i] = &targs[i];
  }
  pthread_t *threads = new_thread_array(2);
  debug("Creating threads...\n");
  create_threads(threads, 2, NULL, &thread_msgfunc, pass);
  join_threads(threads, 2, NULL);

  free(targs);
  free(threads);
  close_queue(mqueue);
  mq_unlink(nid(qid));
  return 0;
}

int main()
 {
  int rc = 0;
  printf("Running L1 tests...\n");
  debug("Debug messages enabled.\n");
  rc += memtest(5);
  rc += messagetest();
  rc += threadtest(5);
  rc += thread_msgtest();
  rc += thread_memtest(5);
  printf("L1 tests complete. %d tests failed.\n", rc);
  return rc;
}
