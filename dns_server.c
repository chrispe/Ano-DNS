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
real DNS server provider what he/she visits. */

#include "./include/udp_listener.h" 
#define DEFAULT_DNS_PORT 53

int main( int argc , char *argv[])
{
	unsigned short port = DEFAULT_DNS_PORT;

	if(argc==3 && strcmp(argv[1],"-p")==0){
		port = atoi(argv[2]);
		if(port==0){
			port = DEFAULT_DNS_PORT;
			fprintf(stdout,"Invalid port, will use the default DNS port (53).\n");
		}
	}

	// Loading some properties.
	read_properties_file("anodns.properties");

    // Get the DNS servers from the resolv.conf file.
    //get_dns_servers();

    // Start listening to the given port for any UDP packet
    // (The followng function includes the query processing).
    udp_listen(port);
 
    return 0;
}