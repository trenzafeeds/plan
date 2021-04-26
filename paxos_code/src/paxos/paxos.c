/************************************
 * paxos.c
 *
 * The single unified Paxos
 * function is used in all
 * three implementations, respecting
 * the classical structure of Paxos.
 * Each function called below is
 * defined seperately according to
 * implementation.
 ************************************/

#include "paxos.h"

int paxos(proc_info self)
{
  int m_type;
  if ((m_type = communicate(self))) {
    switch(m_type) {
    case MSG_PREP:
      acc_prep(self);
      break;
    case MSG_PROP:
      accept(self);
      break;
    case MSG_ACC:
      debug("Thread %d received ACC\n", self->pid);
      count_acc(self);
      break;
    case MSG_LEADER:
      debug("Thread %d received LEADER\n", self->pid);
      if (leader_count(self))
	return self->pid;
      break;
    case MSG_END:
      debug("Thread %d received END\n", self->pid);
      return self->accepted_val;
    default:
      if (m_type >= MSG_PROM) {
	debug("Thread %d received PROM\n", self->pid);
	if (acc_prom(self))
	  propose(self);
      } else {
	debug("Message type received %d\n", m_type);
	app_error("Unrecognized message type.", 1);
      }
    }
  } else {
    if (self->role != ACCEPT) {
      prepare(self);
    }
  }
  return -1;
}
