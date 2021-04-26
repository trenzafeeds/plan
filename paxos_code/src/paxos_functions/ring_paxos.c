/**********************************
 * ring_paxos.c
 *
 * Implementation of parallel
 * message-passing Paxos in a ring
 * structured network.
 **********************************/

#include "paxos_functions.h"
#include "messages.h"

/* The following macro calculates the id of the
 * next process in the ring, for addressing purposes.
 */
#define NEXT(id, M) (((id - FIRSTID + 1) % M) + FIRSTID)

int communicate(proc_info self)
{
  message new_message;
  if ((new_message = receive_m(self->listen)) == -1) {
    return 0;
  }
  else {
    self->new_m = new_message;
    return (self->new_m)->type;
  }
}

int acc_prep(proc_info self)
{
  message current = self->new_m;
  if (current->id > self->promised_id) {
    self->role = ACCEPT;
    message promise = new_message(MSG_PROM + current->id, self->accepted_id,\
				  self->accepted_val, self->pid);
    int dest = NEXT(self->pid, self->M);
    send_m(promise, &dest, 1);
    self->promised_id = current->id;
  }

  free(current);
  return 0;
}

int accept(proc_info self)
{
  message current = self->new_m;
  if (current->id >= self->promised_id) {
    self->role = ACCEPT;
    self->promised_id = current->id;
    self->accepted_id = current->id;
    self->accepted_val = current->val;
    message accept = new_message(MSG_ACC, self->accepted_id,	\
			       self->accepted_val, self->pid);
    int dest = NEXT(self->pid, self->M);
    send_m(accept, &dest, 1);
  }
  free(current);
  return self->accepted_val;
}

int count_acc(proc_info self)
{
  message current = self->new_m;
  if (current->id >= self->promised_id) {
    if ((current->id == self->promised_id) && current->val == self->proposed_val) {
      self->accepted_id = self->promised_id;
      self->accepted_val = self->proposed_val;
      message teach = new_message(MSG_LEADER, self->accepted_id,\
				  self->accepted_val, self->pid);
      int dest = NEXT(self->pid, self->M);
      send_m(teach, &dest, 1);
      free(current);
    } else {
      accept(self);
    }
  } else {
    free(current);
  }
  return 0;
}

int acc_prom(proc_info self)
{
  message current = self->new_m;
  message promise;
  int promised_id = current->type - MSG_PROM;
  if (promised_id > self->promised_id) {
    self->role = ACCEPT;
    if (self->accepted_id > current->id) {
      promise = new_message(MSG_PROM + promised_id, self->accepted_id,\
				    self->accepted_val, self->pid);
    } else {
      promise = new_message(MSG_PROM + promised_id, current->id,\
				    current->val, self->pid);
    }
    int dest = NEXT(self->pid, self->M);
    send_m(promise, &dest, 1);
  } else if ((promised_id == self->promised_id) && (self->role == PREPARE)) {
    self->highest_old_id = current->id;
    self->old_id_val = current->val;
    free(current);
    return 1;
  }
  free(current);
  return 0; 
}

int prepare(proc_info self)
{
  if (self->role != PROPOSE) {
    self->role = PREPARE;
    while (RID(self->round, self->pid, self->M) < self->promised_id)
      self->round++;
    self->promised_id = RID(self->round, self->pid, self->M);
    message prepare = new_message(MSG_PREP,RID(self->round, self->pid, self->M),\ 
				  NO_VAL, self->pid);
    int dest = NEXT(self->pid, self->M);
    send_m(prepare, &dest, 1);
    debug("Thread %d sent PREPARE.\n", self->pid);
  }
  return 0;
}

int propose(proc_info self)
{
  self->role = PROPOSE;
  if (self->old_id_val != NO_VAL)
    self->proposed_val = self->old_id_val;
  else self->proposed_val = self->pid;
  self->highest_old_id = NO_VAL;
  self->old_id_val = NO_VAL;
  message proposal = new_message(MSG_PROP, self->promised_id, self->proposed_val,\
				 self->pid);
  int dest = NEXT(self->pid, self->M);
  send_m(proposal, &dest, 1);
  debug("Thread %d sent PROPOSE.\n", self->pid);
  return 0;
}

int leader_count(proc_info self)
{
  message current = self->new_m;
  if (current->val != self->pid) {
    int dest = NEXT(self->pid, self->M);
    send_m(current, &dest, 1);
    return 0;
  } else {
    message end = new_message(MSG_END, 0, 0, self->pid);
    send_all(end, self->pid, FIRSTID, self->M);
    free(current);
    return 1;
  }
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
