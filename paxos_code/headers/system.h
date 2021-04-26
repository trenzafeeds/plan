/*********************
 * system.h
 * 
 * Master header
 *********************/

/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>

/* Local Library */
#include "utils.h"

#define FIRSTID 101
#define MAXNODES 8

#ifndef SYSHEADER
#define SYSHEADER

#define DEBUG

#endif /* SYSHEADER */
