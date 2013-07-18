#include "udp_listener.h" 

#define DNS_DEFAULT_PORT 53

int main( int argc , char *argv[])
{
    //Get the DNS servers from the resolv.conf file
    get_dns_servers();
     
    udp_listen(DNS_DEFAULT_PORT);
 
    return 0;
}