/************
 * threads.h
 ************/

#include "system.h"

#ifndef THREADSHEADER
#define THREADSHEADER

#define MAXTHREADS 20

pthread_t *new_thread_array(int n);
int spthread_create(pthread_t *thread, const pthread_attr_t *attr,
		    void *(*start_routine) (void *), void *arg);
int spthread_join(pthread_t thread, void **retval);
int create_threads(pthread_t *thread_arr, int n, const pthread_attr_t *attr,
		   void *(*start_routine) (void *), void **arg_arr);
int join_threads(pthread_t *thread_arr, int n, void **ret_arr);

#endif /* THREADSHEADER */
