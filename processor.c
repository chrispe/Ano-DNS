#include "processor.h"

#define MAX_SEND_TRIES 10

char dns_servers[10][100];
int dns_server_count = 0;

/**
 * @brief Handles the query processing being under a thread.
 * @param thread_parameters : The thread parameters (query parameters).
 */
void * handle_query (void * thread_parameters){
  	query_thread_params * query_params = (query_thread_params *)thread_parameters;
  	ngethostbyname(query_params->domain,T_A,&(query_params->sock),query_params->addr,query_params->len,query_params->query_id);
 	pthread_exit(NULL);
}

/**
 * @brief Returns the domain name fixed (with dots).
 * @param reader : The pointer to the wrong domain name.
 * @param buffer : The pointer ro the buffer (where all the packet data is).
 * @return: A pointer to a new string with the valid domain name.
 */
u_char * get_query_domain(unsigned char * reader,unsigned char * buffer)
{
    unsigned char * name;
    unsigned int p = 0, offset;
    int i , j;

    name = (unsigned char *)malloc(256);
    name[0]='\0';

    while(*reader!=0){
        if(*reader>=192){
            offset = (*reader)*256 + *(reader+1) - 49152;  
            reader = buffer + offset - 1;
        }
        else
            name[p++]=*reader;
        reader++;
    }
 
    name[p]='\0';

    unsigned int len = strlen(name);
    for(i=0;i<len;i++){
        p = name[i];
        for(j=0;j<(int)p;j++){
            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    name[i-1]='\0';
    return name;
}

/**
 * @brief Sends the query to another remote DNS server and then sends
 * back to the client the received response.
 * @param thread_parameters : The thread parameters (query parameters).
 */
void ngethostbyname(unsigned char * host , int query_type, int * sock, struct sockaddr_in * c_addr,unsigned int len, unsigned short q_id)
{
    unsigned char buf[65536], * qname;
    int i , s;
    struct sockaddr_in a;
    struct sockaddr_in dest;
    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;
 
    printf("Resolving domain (%s)\n" , host);
 
 	// For sending the query through UDP.
    s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);
 
 	// Setting up the destination of the UDP message.
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(dns_servers[0]); //dns servers
 
    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *)&buf;
    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; 				// This is a query
    dns->opcode = 0; 			// This is a standard query
    dns->aa = 0; 				// Not Authoritative
    dns->tc = 0; 				// This message is not truncated
    dns->rd = 1; 				// Recursion Desired
    dns->ra = 0; 				// Recursion not available
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1);	// We have only one question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;
 
    // Setting up the DNS query to send.
    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];
    ChangetoDnsNameFormat(qname , host);
    qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];
    qinfo->qtype = htons( query_type ); // Type of the query
    qinfo->qclass = htons(1); 			// For the internet
 
    // Sending the query to the DNS server.
    send_udp_packet(&s,(char*)buf,sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION),(struct sockaddr*)&dest,sizeof(dest));

    // Receiving the reponse from the DNS server.
    i = sizeof(dest);
    if(recvfrom (s,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
        return;
 
    // Copying the client's ID to the beginning of the buffer (where the DNS packet starts).
    // We do this because we have received the response from another DNS server so,
    // the ID on the packet is the ID that we just used to make the query.
    memcpy(&buf,&q_id,sizeof(unsigned short)); 

    // Set the packet to send back to the client who made the query.
    char * packet = malloc(512);
    if(memerror(packet,"new client packet"))
    	return;

    memset(packet,0,512);
    memcpy(packet,buf,512);
    send_udp_packet(sock,packet,512,(struct sockaddr *)c_addr,len);
    free(packet);
}

/**
 * @brief Sends the packet using the stantard sendto function but 
 * we perform a number of tries (MAX_SEND_TRIES) in case we failed to send the packet.
 * @param the sendto() parameters. Read sendto() documentation.
 */
void send_udp_packet(int * sock, char * content,unsigned int size, struct sockaddr * addr, unsigned int len){
	unsigned short tries = 0;
	for(tries=0;tries<MAX_SEND_TRIES;tries++){
		if(sendto(*sock,content,size,0,addr,len) >= 0)
			return;
	}
}

 
/*
 *
 * */
u_char* ReadName(unsigned char* reader,unsigned char* buffer,int* count)
{
    unsigned char *name;
    unsigned int p=0,jumped=0,offset;
    int i , j;
 
    *count = 1;
    name = (unsigned char*)malloc(256);
 
    name[0]='\0';
 
    //read the names in 3www6google3com format
    while(*reader!=0)
    {
        if(*reader>=192)
        {
            offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        }
        else
        {
            name[p++]=*reader;
        }
 
        reader = reader+1;
 
        if(jumped==0)
        {
            *count = *count + 1; //if we havent jumped to another location then we can count up
        }
    }
 
    name[p]='\0'; //string complete
    if(jumped==1)
    {
        *count = *count + 1; //number of steps we actually moved forward in the packet
    }
 
    //now convert 3www6google3com0 to www.google.com
    for(i=0;i<(int)strlen((const char*)name);i++)
    {
        p=name[i];
        for(j=0;j<(int)p;j++)
        {
            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    name[i-1]='\0'; //remove the last dot
    return name;
}
 
/*
 * Get the DNS servers from /etc/resolv.conf file on Linux
 * */
void get_dns_servers()
{
    FILE *fp;
    char line[200] , *p;
    if((fp = fopen("/etc/resolv.conf" , "r")) == NULL)
    {
        printf("Failed opening /etc/resolv.conf file \n");
    }
     
    while(fgets(line , 200 , fp))
    {
        if(line[0] == '#')
        {
            continue;
        }
        if(strncmp(line , "nameserver" , 10) == 0)
        {
            p = strtok(line , " ");
            p = strtok(NULL , " ");
             
            //p now is the dns ip :)
            //????
        }
    }
     
    strcpy(dns_servers[0] , "208.67.222.222");
    strcpy(dns_servers[1] , "208.67.220.220");
}
 
/*
 * This will convert www.google.com to 3www6google3com
 * got it :)
 * */
void ChangetoDnsNameFormat(unsigned char* dns,unsigned char* host)
{
    int lock = 0 , i;
    strcat((char*)host,".");
     
    for(i = 0 ; i < strlen((char*)host) ; i++)
    {
        if(host[i]=='.')
        {
            *dns++ = i-lock;
            for(;lock<i;lock++)
            {
                *dns++=host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++='\0';
}