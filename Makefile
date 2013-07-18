# This is the makefile for the linux version of the project Ano-DNS.

LINKED_OBJ = dnss.o proc.o udplin.o
LIBS = -pthread
OUTPUT = ./dns_server

all: out 
out: $(LINKED_OBJ)
	@echo Building executable file...
	@gcc -o $(OUTPUT) $(LINKED_OBJ) $(LIBS)
	@echo Cleaning up...
	@rm -f $(LINKED_OBJ)
	@echo Done.
dnss.o: dns_server.c
	@gcc -o dnss.o -c dns_server.c 
proc.o: processor.h processor.c sys_libs.h
	@gcc -o proc.o -c processor.c 
udplin.o: udp_listener.h udp_listener.c dns_packet.h sys_libs.h
	@gcc -o udplin.o -c udp_listener.c
clean:
	@echo Cleaning up...
	@rm -f $(LINKED_OBJ)
	@rm -f $(OUTPUT)
	@echo Done.
