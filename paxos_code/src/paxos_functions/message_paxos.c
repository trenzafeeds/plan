/********************************
 * message_paxos.c
 * 
 * Implementation of distributed
 * message-passing Paxos.
 ********************************/

#include "paxos_functions.h"
#include "messages.h"

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
    int *dest = &current->auth;
    send_m(promise, dest, 1);
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
    message accept = new_message(MSG_ACC, self->accepted_id,\
				self->accepted_val, self->pid);
    int *dest = &current->auth;
    send_m(accept, dest, 1);
    message teach = new_message(MSG_LEADER, self->accepted_id,\
				self->accepted_val, self->pid);
    dest = &self->accepted_val;
    send_m(teach, dest, 1);
  }
  free(current);
  return self->accepted_val;
}

int count_acc(proc_info self)
{
  message current = self->new_m;
  if (current->id == self->promised_id) {
    self->role = PROPOSE;
    self->count++;
    if (self->count > (self->M / 2)) {
      self->accepted_id = self->promised_id;
      self->accepted_val = self->proposed_val;
      message teach = new_message(MSG_LEADER, self->accepted_id,\
				  self->accepted_val, self->pid);
      int *dest = &self->accepted_val;
      send_m(teach, dest, 1);
      self->role = ACCEPT;
    }
  }
  return 0;
}

int acc_prom(proc_info self)
{
  message current = self->new_m;
  int promise_id = current->type - MSG_PROM;
  if ((promise_id == self->promised_id) && (self->role == PREPARE)) {
    self->count++;
    if (current->id > self->highest_old_id) {
      self->highest_old_id = current->id;
      self->old_id_val = current->val;
    }
  }
  free(current);
  if ((self->count > (self->M / 2)) && (self->role == PREPARE))
    return 1;
  return 0;
}

int prepare(proc_info self)
{
  self->role = PREPARE;
  self->count = 0;
  
  while (RID(self->round, self->pid, self->M) < self->promised_id) {
    self->round++;
  }
  
  self->promised_id = RID(self->round, self->pid, self->M);
  message prepare = new_message(MSG_PREP, RID(self->round, self->pid, self->M),\
				NO_VAL, self->pid);
  send_all(prepare, self->pid, FIRSTID, self->M);
  debug("Thread %d sent PREPARE\n", self->pid);
  return 0;
}

int propose(proc_info self)
{
  self->role = PROPOSE;
  self->count = 0;
  if (self->old_id_val != NO_VAL)
    self->proposed_val = self->old_id_val;
  else self->proposed_val = self->pid;
  self->highest_old_id = NO_VAL;
  self->old_id_val = NO_VAL;
  message proposal = new_message(MSG_PROP, self->promised_id, self->proposed_val,\
				 self->pid);
  send_all(proposal, self->pid, FIRSTID, self->M);
  debug("Thread %d sent PROPOSE\n", self->pid);
  return 0;
}

int leader_count(proc_info self)
{
  free(self->new_m);
  self->leader_count++;
  if (self->leader_count > self->M/2) {
    debug("Thread %d: I AM LEADER. Commanding end.\n", self->pid);
    message end = new_message(MSG_END, 0, 0, self->pid);
    send_all(end, self->pid, FIRSTID, self->M);
    return 1;
  }
  return 0;
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

