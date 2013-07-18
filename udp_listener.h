/*
===========================================
=                                         =
=                 Ano-DNS                 =
=               version 0.1               =
=                                         =
=    Developed by Christos Petropoulos    =
=                                         =
===========================================

This header file includes the structures and
functions used for receiving and sending packets
using the UDP protocol.
*/

#include "dns_packet.h"
#include "processor.h"

/**
 * @brief Binds to all the available network interfaces and starts 
 * listening for any UDP message on the given port.
 * @param listening_port : The port that we are listening to.
 */
void udp_listen(unsigned short listening_port);

/* 	This struct includes the variables needed
	to complete the query under a new thread. */
typedef struct query_thread_params {
  int sock;						// the socket used with the client
  unsigned int len;				// the address length
  struct QUESTION info;			// the question of the query
  char * domain;				// the domain name of the query
  struct sockaddr_in * addr;	// the addr of the client
  unsigned short query_id;		// the id of the query
}query_thread_params;

/**
 * @brief Allocates and creates a new query_thread_params.
 * @return A pointer in case the allocation was successful else NULL.
 */
query_thread_params * new_query_params(void);

/**
 *	@brief Destroys the query_thread_params object. 
 	@param q: The reference to the object we want to destroy. 
 */
void destroy_query_params(query_thread_params * q){

/**
 * 	@brief Checks if the reference points to NULL.
	in case it does, an error is printed on the stdout.
	@param p: the pointer we want to check.
	@param obj_name: the name of object we try to allocate. 
	@return: 1 for error, zero for no error.
 */ 
char memerror(void * p, char * obj_name);