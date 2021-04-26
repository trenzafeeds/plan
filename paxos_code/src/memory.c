/****************************
 * memory.c
 *
 * Functions for managing
 * communication of threads
 * using shared memory.
 ****************************/

#include "memory.h"

struct block *new_block()
{
  return scalloc(1, B_SIZE);
}

shared_memory new_shared_mem(int N)
{
  return scalloc(N, B_SIZE);
}

/* The following functions do not throw errors, but return -1 on
 * failed memory operations.
 */
int read_block(struct block *container, int id, shared_memory shared_block)
{
  if (memcpy(container, &shared_block[id], B_SIZE) == NULL) {
    debug("Failed to read from block %n.\n", id);
    return -1;
  } else
    return 0;
}

int write_block(struct block *data, int id, shared_memory shared_block)
{
  if (memcpy(&shared_block[id], data, B_SIZE) == NULL) {
    debug("Failed to write to block %n.\n", id);
    return -1;
  } else
    return 0;
}

int wipe_block(struct block *target)
{
  if (memset(target, 0, B_SIZE) == NULL) {
    debug("Failed to wipe block.\n");
    return -1;
  } else
    return 0;
}
