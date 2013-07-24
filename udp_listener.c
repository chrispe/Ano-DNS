#include "udp_listener.h"

/**
 * @brief Binds to all the available network interfaces and starts 
 * listening for any UDP message on the given port.
 * @param listening_port : The port that we are listening to.
 */
void udp_listen(unsigned short listening_port){
  int sock;
  char buffer[64];
  unsigned int addr_len, bytes_read;
  struct sockaddr_in server_addr, client_addr;
  unsigned short * id = NULL;
  unsigned char * qname = NULL;
  pthread_t * query_thread = NULL;
  pthread_attr_t thread_attributes;

  // Initializing the thread attributes
  pthread_attr_init(&thread_attributes);
  pthread_attr_setdetachstate(&thread_attributes,PTHREAD_CREATE_DETACHED);

  printf("Opening UDP socket on port %d...",listening_port);

  // We create the socket for UDP
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    printf("(FAILED)\n\t");
    perror("opening UDP socket");
    return;
  }

  // We set parameters for the connection
  memset(&server_addr,0,sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(listening_port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Bind it to all the available interfaces
  if(bind(sock,(struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1) {
    printf("(FAILED)\n\t");
    perror("UDP Bind");
    return;
  }

  #ifdef __linux
    printf("(DONE)\nStarted listening for UDP packets on %s:%d...\n",get_local_ip(),listening_port);
  #else
    printf("(DONE)\nStarted listening for UDP packets on %d...\n",listening_port);
  #endif

  fflush(stdout);

  while(1){

    // Set the buffer all to zero.
    memset(&buffer,0,64);

    // Allocate space for the new query parameters.
    query_thread_params * query_params = new_query_params();
    if(!query_params)
      continue;

    // Waiting for a UDP packet to arrive.
    addr_len = sizeof(struct sockaddr_in);
    if((bytes_read = recvfrom(sock,buffer,63,0,(struct sockaddr *)&client_addr,&addr_len)) == -1){
      perror("UDP read");
      return;
    }
    buffer[bytes_read] = 0;
    
    // We copy the id of the query to a temporary variable.
    id = malloc(sizeof(unsigned short));
    if(memerror(id,"query id")){
      destroy_query_params(query_params);
      continue;
    }
    memcpy(id,buffer,sizeof(unsigned short));

    // Set the query thread parameters.
    query_params->sock = sock;
    query_params->len = addr_len; 
    
    // Get the domain name from the buffer.
    qname = (unsigned char *)&buffer[sizeof(struct DNS_HEADER)];
    // and then set it to the query params but filtered (by placing the dots).
    query_params->domain = get_query_domain(qname,buffer);

    // Set some more parameters
    query_params->info.qtype = buffer[sizeof(struct DNS_HEADER) + (strlen((const char *)qname))+1];
    memcpy(query_params->addr,&client_addr,sizeof(struct sockaddr));
    query_params->query_id  = *id;
    strcpy(query_params->dns_server,dns_servers[0]);

    // We create the thread which handles the dns query
    query_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if(query_thread!=NULL){
      pthread_create(query_thread,&thread_attributes,&handle_query,query_params);
      free(query_thread);
    }
    free(id);
  }
}

/**
 * @brief Allocates and creates a new query_thread_params.
 * @return A pointer in case the allocation was successful else NULL.
 */
query_thread_params * new_query_params(){
  query_thread_params * new_qtp = (query_thread_params *)malloc(sizeof(query_thread_params));
  if(memerror(new_qtp,"query params"))
    return new_qtp;
  new_qtp->domain = malloc(256);
  new_qtp->dns_server = malloc(256);
  new_qtp->addr = malloc(sizeof(struct sockaddr));
  if(memerror(new_qtp,"sockaddr"))
    free(new_qtp);
  return new_qtp;
}

/**
 *  @brief Destroys the query_thread_params object. 
  @param q: The reference to the object we want to destroy. 
 */
void destroy_query_params(query_thread_params * q){
  if(q){
    if(q->addr)
      free(q->addr);
    if(q->domain)
      free(q->domain);
    free(q);
  }
}

/**
 * @brief Checks if the reference points to NULL.
   in case it does, an error is printed on the stdout.
   @param p: the pointer we want to check.
   @param obj_name: the name of object we try to allocate. 
   @return: 1 for error, zero for no error.
 */ 
char memerror(void * p, char * obj_name){
  if(!p){
    fprintf(stdout,"Memory allocation (malloc) failed for : %s\n",obj_name);
    return 1;
  }
  return 0;
}

/**
 *  @brief Returns the IP address of the local host.
    in case it does, an error is printed on the stdout.
    @return: The IP address as a string.
 */ 
char * get_local_ip()
{
  char hostname[130];
  struct hostent *host;
  char * fixed_hostname;

  // We get the hostname of the computer
  gethostname(hostname, sizeof hostname);

  // In the table, the entry of the external address
  // has the name of the hostname plus ".local"
  // So we need to create a string in that format.
  fixed_hostname = malloc(sizeof(hostname)+7);
  sprintf (fixed_hostname, "%s.local",hostname);

  // Then by giving the right name we get the right address
  host = gethostbyname(fixed_hostname);

  if(host == NULL)
  {
    herror("gethostbyname");
    return NULL;
  }
 
  return(inet_ntoa(*(struct in_addr*)host->h_addr));
}
