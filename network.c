#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ifaddrs.h> 
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <stddef.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include "udp_listener.h" 

#define DNS_DEFAULT_PORT 53

int main( int argc , char *argv[])
{
    unsigned char hostname[100];
 
    //Get the DNS servers from the resolv.conf file
    get_dns_servers();
     
    udp_listen(DNS_DEFAULT_PORT);
 
    return 0;
}
  
