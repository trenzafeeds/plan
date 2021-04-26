# General Makefile for small C programs
# Based on one by Alexender Hiam in Jan 2013
# Jim Mahoney | Jan 2016 | Public Domain

############################################################
# Modify these to fit your project.

# The name of the target executable
TARGET  = L1tests

# Space separated list of all source files
SOURCE_FILES = sys/l1tests.c memory.c messages.c threads.c sys/utils.c

# Space separated directories containing source files
INCLUDE_DIRS = ../headers

# Compiler flags (e.g. optimization, links, etc.):
CFLAGS = -O2 -g -Wall -lpthread

# Compiler:
CC = gcc

############################################################
# You probably won't need to change this part.

# Append -I to each include dir
INCLUDES = $(foreach dir, $(INCLUDE_DIRS), -I$(dir))

# Append path to src/ dir
SOURCES = $(foreach file, $(SOURCE_FILES), ../src/$(file))

# Create list of the object files
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all

L1: $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJECTS) -o $(TARGET) -lrt

all:
	make L1

%.o: %.c
	@echo "Compiling $<"
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $< -lrt
