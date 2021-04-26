/*****************************
 * memory_paxos.c
 *
 * Implementation of parallel
 * Paxos on shared memory
 * (Disk Paxos).
 *****************************/

#include "paxos_functions.h"
#include "memory.h"

int communicate(proc_info self)
{
  if (self->role == ACCEPT) {
    sleep(IND(self->pid) % (self->M / 2 + 1));
    self->role = PREPARE;
    return 0;
  } else if (self->cid < self->highest_seen)
    return 0;
  struct block *read = scalloc(1, B_SIZE);
  for (int i; i < self->M; i++) {
    if (i != IND(self->pid)) {
      read_block(read, i, self->smem);
      if (read->cid > self->cid) {
	if (!((self->role == PROPOSE) && (read->val == self->accepted_val))) {
	  self->highest_seen = read->cid;
	  free(read);
	  self->role = ACCEPT;
	  debug("Thread %d entered ACCEPT state.\n", self->pid);
	  return 0;
	}
      }
      else if ((read->id >= self->highest_old_id) && (self->role == PREPARE)) {
	self->highest_old_id = read->id;
	self->old_id_val = read->val;
      }
    }
  }
  free(read);
  if (self->role == PREPARE) {
    self->role = PROPOSE;
    return MSG_PROM;
  } else {
    return MSG_END;
  }
}

int acc_prep(proc_info self)
{
  return 1;
}

int accept(proc_info self)
{
  return 1;
}

int count_acc(proc_info self)
{
  return 1;
}

int acc_prom(proc_info self)
{
  return 1;
}

int prepare(proc_info self)
{
  self->role = PREPARE;
  self->cid = RID(self->round, self->pid, self->M);
  while (self->cid < self->highest_seen) {
    self->round++;
    self->cid = RID(self->round, self->pid, self->M);
  }
  struct block *write = scalloc(1, B_SIZE);
  write->cid = self->cid;
  write_block(write, IND(self->pid), self->smem);
  free(write);
  debug("Thread %d entered PREPARE state.\n", self->pid);
  return 0;
}

int propose(proc_info self)
{
  self->role = PROPOSE;
  self->accepted_id = self->cid;
  if (self->old_id_val != NO_VAL)
    self->accepted_val = self->old_id_val;
  else
    self->accepted_val = self->pid;
  struct block *write = scalloc(1, B_SIZE);
  write->cid = self->cid;
  write->id = self->accepted_id;
  write->val = self->accepted_val;
  write_block(write, IND(self->pid), self->smem);
  free(write);
  debug("Thread %d entered PROPOSE state.\n", self->pid);
  return 0;
}

int leader_count(proc_info self)
{
  return 1;
}

int fail(proc_info self)
{
  srandom(random());
  if ((random() % 23) == 0) {
    debug("Thread %d failed!\n", self->pid);
    sleep(4);
  }
  return 0;
}
