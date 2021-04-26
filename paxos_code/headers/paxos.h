/**************
 * paxos.h
 **************/

/*******************************************
 * Prototypes to be defined uniquely
 * for each model.
 *******************************************
 * struct proc_info
 *     listen : message queue or memory block
 * 
 * int communicate(proc_info)
 *     reads message or memory into proc_info local
 *     returns message_type if applicable, else 0
 * acc_prep(proc_info)
 * accept(proc_info)
 * count_acc(proc_info)
 * acc_prom(proc_info)
 *     returns true if majority is reached
 *     else returns false
 * propose(self)
 * prepare(self)
 *******************************************/

#include "paxos_functions.h"

#ifndef PAXOSHEADER
#define PAXOSHEADER

int paxos(proc_info self);

#endif /* PAXOSHEADER */
