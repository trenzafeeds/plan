######################################
#
# Top level Makefile for controlling
# sub-makefiles
#
######################################

.PHONY: clean tests messages ring memory all

all:
	@make messages
	@make ring
	@make memory
	@make tests
	@make clean

messages:
	@make clean
	@cd bin && make messages

ring:
	@make clean
	@cd bin && make ring

memory:
	@make clean
	@cd bin && make memory

tests:
	@make clean
	@cd tests && make all

clean:
	rm -f src/*.o src/nodes/*.o src/paxos/*.o src/sys/*.o


