/*
* * * * * * * * * * * * * * * * * * * * * *
*                                         *
*                 Ano-DNS                 *
*               version 0.1               *
*                                         *
*    Developed by Christos Petropoulos    *
*                                         *
* * * * * * * * * * * * * * * * * * * * * *

This header file includes the functions used
for processing the query the client has sent.
*/

#include "sys_libs.h"
#include "binary_tree.h"
#ifndef DNS_PACKET_SRUCT
#include "dns_packet.h"
#endif

#define MAX_DNS_SERVERS 10
#define MAX_SEND_TRIES 10

typedef enum prop_type{
	BLOCK_ITEM,
	SERVER_ITEM,
	NONE
}prop_type;

/* 	An array including some dns servers
	which will be used for making the queries. */
extern char dns_servers[MAX_DNS_SERVERS][100];
  
/* 	An array including the domain names
	which are blocked. When a blocked domain
	name has been requested to be resolved,
	we don't make the query to any DNS server.
	We just return an answer with a user defined value. 
*/
extern tree_node * blocked_domains_tree;
 
/**
 * @brief Sends the query to another remote DNS server and then sends
 * back to the client the received response.
 * @param thread_parameters : The thread parameters (query parameters).
 */
void process_query(unsigned char * host, const char * dns_server ,int query_type, int * sock, struct sockaddr_in * c_addr,unsigned int len, unsigned short q_id);

/**
 * @brief Converts from the dot format to the number format.
 * @param dns: the output we are going to use for making the query.
 * @param host: the original name of the host (which uses dots).
 */
void host_to_dns_format (unsigned char *, unsigned char *);
 
/**
 * @brief Gets the default DNS servers (works only on linux).
 * @return: Returns (-1) if the function was unable to retrieve the dns servers. 
 * else returns zero.
 */ 
unsigned short  get_dns_servers(void);

/**
 * @brief Handles the query processing being under a thread.
 * @param thread_parameters : The thread parameters (query parameters).
 */
void * handle_query(void * thread_parameter);

 /**
 * @brief Returns the domain name fixed (with dots).
 * @param reader : The pointer to the wrong domain name.
 * @param buffer : The pointer ro the buffer (where all the packet data is).
 * @return: A pointer to a new string with the valid domain name.
 */
u_char * get_query_domain(unsigned char * reader, unsigned char * buffer);

/**
 * @brief Sends the packet using the stantard sendto function but 
 * we perform a number of tries (MAX_SEND_TRIES) in case we failed to send the packet.
 * @param the sendto() parameters. Read sendto() documentation.
 */
void send_udp_packet(int * sock, char * content,unsigned int size, struct sockaddr * addr, unsigned int len);

u_char* ReadName(unsigned char* reader,unsigned char* buffer,int* count);
void ngethostbyname(unsigned char * buf ,struct DNS_HEADER * dns);
void read_properties_file(const char * fname);