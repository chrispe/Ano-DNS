/*
* * * * * * * * * * * * * * * * * * * * * *
*                                         *
*                 Ano-DNS                 *
*               version 0.1               *
*                                         *
*    Developed by Christos Petropoulos    *
*                                         *
* * * * * * * * * * * * * * * * * * * * * *

Ano-DNS acts as a DNS server.
However, it is not a real DNS server.
What Ano-DNS does is to mediate a client's 
query to a real DNS server and then send 
the results back to the client.

By this way the user can "hide" to the
real DNS server provider what he/she visits.
*/

#include "udp_listener.h" 

#define DNS_DEFAULT_PORT 53

int main( int argc , char *argv[])
{
    // Get the DNS servers from the resolv.conf file
    get_dns_servers();
     
    // Start listening to the given port for any UDP packet
    // (The followng function includes the query processing).
    udp_listen(DNS_DEFAULT_PORT);
 
    return 0;
}