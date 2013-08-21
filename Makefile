# This is the makefile for the linux version of the project Ano-DNS.

LINKED_OBJ = dnss.o proc.o udplin.o btr.o
LIBS = -pthread
OUTPUT = ./bin/dns_server
CUST_LIBS = ./include/

all: out 
out: $(LINKED_OBJ)
	@echo Building executable file...
	@gcc -o $(OUTPUT) $(LINKED_OBJ) $(LIBS)
	@echo Cleaning up...
	@rm -f $(LINKED_OBJ)
	@echo Done.
btr.o: $(CUST_LIBS)binary_tree.h binary_tree.c
	@gcc -o btr.o -c binary_tree.c
dnss.o: dns_server.c
	@gcc -o dnss.o -c dns_server.c 
proc.o: $(CUST_LIBS)processor.h processor.c $(CUST_LIBS)sys_libs.h
	@gcc -o proc.o -c processor.c 
udplin.o: $(CUST_LIBS)udp_listener.h udp_listener.c $(CUST_LIBS)dns_packet.h $(CUST_LIBS)sys_libs.h
	@gcc -o udplin.o -c udp_listener.c
clean:
	@echo Cleaning up...
	@rm -f $(LINKED_OBJ)
	@rm -f $(OUTPUT)
	@echo Done.
