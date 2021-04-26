/***************************
 * threads.c
 *
 * Functions managing the
 * creating and joining of
 * pthreads. To be used
 * by the top-level driver
 * program.
 ***************************/

#include "threads.h"

pthread_t *new_thread_array(int n)
{
  return scalloc(n, sizeof(pthread_t));
}

int spthread_create(pthread_t *thread, const pthread_attr_t *attr,
		    void *(*start_routine) (void *), void *arg)
{
  int ret;
  if ((ret = pthread_create(thread, attr, start_routine, arg)) != 0)
    sys_error("Error at pthread_create.", ret);
  return 0;
}

int spthread_join(pthread_t thread, void **retval)
{
  int ret;
  if ((ret = pthread_join(thread, retval)) != 0)
    sys_error("Error at pthread_join.", ret);
  return 0;
}

int create_threads(pthread_t *thread_arr, int n, const pthread_attr_t *attr,
		   void *(*start_routine) (void *), void **arg_arr)
{
  for (int i = 0; i < n; i++) {
    spthread_create(&thread_arr[i], attr, start_routine, arg_arr[i]);
  }
  return 0;
}

int join_threads(pthread_t *thread_arr, int n, void **ret_arr)
{
  for (int i = 0; i < n; i++) {
    if (ret_arr != NULL)
      spthread_join(thread_arr[i], ret_arr[i]);
    else {
      //debug("Driver attempting to join thread %d\n", i);
      spthread_join(thread_arr[i], NULL);
      //debug("Driver joined thread %d\n", i);
    }
  }
  return 0;
}



