/*
* * * * * * * * * * * * * * * * * * * * * *
*                                         *
*                 Ano-DNS                 *
*               version 0.1               *
*                                         *
*    Developed by Christos Petropoulos    *
*                                         *
* * * * * * * * * * * * * * * * * * * * * *

This header file includes the structures and
functions used for receiving UDP packets.
*/

#include "sys_libs.h"
#include "dns_packet.h"
#include "processor.h"

/**
 * @brief Binds to all the available network interfaces and starts 
 * listening for any UDP message on the given port.
 * @param listening_port : The port that we are listening to.
 */
void udp_listen(unsigned short listening_port);

/**
 * @brief Allocates and creates a new query_thread_params.
 * @return A pointer in case the allocation was successful else NULL.
 */
query_thread_params * new_query_params(void);

/**
 *	@brief Destroys the query_thread_params object. 
 	@param q: The reference to the object we want to destroy. 
 */
void destroy_query_params(query_thread_params * q);

/**
 * 	@brief Checks if the reference points to NULL.
	in case it does, an error is printed on the stdout.
	@param p: the pointer we want to check.
	@param obj_name: the name of object we try to allocate. 
	@return: 1 for error, zero for no error.
 */ 
char memerror(void * p, char * obj_name);