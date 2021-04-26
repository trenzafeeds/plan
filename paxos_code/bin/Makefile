# General Makefile for small C programs
# Based on one by Alexender Hiam in Jan 2013
# Jim Mahoney | Jan 2016 | Public Domain

############################################################
# Modify these to fit your project.

# Functions
prep_path = $(foreach path, $(2), $(1)/$(path))

# Space separated directories containing source files
INCLUDE_DIRS = ../headers
SRC_DIR = ../src

SYS_SOURCES = sys/utils.c threads.c paxos/driver.c paxos/paxos.c
SYS_ALL := $(call prep_path, $(SRC_DIR), $(SYS_SOURCES))

MESSAGE_T = message_paxos
MESSAGE_SOURCES = messages.c paxos_functions/message_paxos.c \
nodes/message_node.c
MESSAGE_ALL = $(SYS_ALL) $(call prep_path, $(SRC_DIR), $(MESSAGE_SOURCES))

RING_T = ring_paxos
RING_SOURCES = messages.c paxos_functions/ring_paxos.c \
nodes/message_node.c
RING_ALL = $(SYS_ALL) $(call prep_path, $(SRC_DIR), $(RING_SOURCES))

MEM_T = memory_paxos
MEM_SOURCES = memory.c paxos_functions/memory_paxos.c \
nodes/memory_node.c
MEM_ALL = $(SYS_ALL) $(call prep_path, $(SRC_DIR), $(MEM_SOURCES))

# Compiler flags (e.g. optimization, links, etc.):
CFLAGS = -O2 -g -Wall -lrt -lpthread

# Compiler:
CC = gcc

############################################################
# You probably won't need to change this part.

# Append -I to each include dir
INCLUDES := $(foreach dir, $(INCLUDE_DIRS), -I$(dir))
CFLAGS += $(INCLUDES)

# Create list of the object files
MESSAGE_OBJ = $(MESSAGE_ALL:.c=.o)
RING_OBJ = $(RING_ALL:.c=.o)
MEM_OBJ = $(MEM_ALL:.c=.o)

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

memory : CFLAGS += -D MEMORY -include memory.h
memory : $(MEM_OBJ)
	$(CC) $(CFLAGS) $(MEM_OBJ) -o $(MEM_T)

messages : CFLAGS += -include messages.h
messages : $(MESSAGE_OBJ)
	$(CC) $(CFLAGS) $(MESSAGE_OBJ) -o $(MESSAGE_T)

ring : CFLAGS += -include messages.h
ring : $(RING_OBJ)
	$(CC) $(CFLAGS) $(RING_OBJ) -o $(RING_T)
