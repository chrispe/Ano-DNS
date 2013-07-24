#include "processor.h"

/*  An array including some dns servers
    which will be used for making the queries. */
char dns_servers[MAX_DNS_SERVERS][100];
unsigned short dns_servers_count = 0;

/*  An array including the domain names
    which are blocked. When a blocked domain
    name has been requested to be resolved,
    we don't make the query to any DNS server.
    We just return an answer with a user defined value. */
tree_node * blocked_domains_tree = NULL;
 
/**
 * @brief Handles the query processing being under a thread.
 * @param thread_parameters : The thread parameters (query parameters).
 */
void * handle_query (void * thread_parameters){
    query_thread_params * query_params = (query_thread_params *)thread_parameters;
    process_query(query_params->domain,query_params->dns_server,T_A,&(query_params->sock),query_params->addr,query_params->len,query_params->query_id);
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
void process_query(unsigned char * host, const char * dns_server ,int query_type, int * sock, struct sockaddr_in * c_addr,unsigned int len, unsigned short q_id)
{
    unsigned char buf[512], * qname;
    int i , s;
    struct sockaddr_in a;
    struct sockaddr_in dest;
    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;
    unsigned int packet_size;
    char is_blocked = 0;
    memset(buf,512,0);
    
    printf("Resolving domain (%s)\n" , host);
 
    // For sending the query through UDP.
    s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);
 
    // Setting up the destination of the UDP message.
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(dns_servers[0]);
 
    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *)&buf;
    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0;                // This is a query
    dns->opcode = 0;            // This is a standard query
    dns->aa = 0;                // Not Authoritative
    dns->tc = 0;                // This message is not truncated
    dns->rd = 1;                // Recursion Desired
    dns->ra = 0;                // Recursion not available
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1);    // We have only one question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;
 
    if(tree_lookup(blocked_domains_tree,host)!=NULL){
        dns->rcode = 3;
        memcpy(buf+sizeof(unsigned short)+1,dns,sizeof(struct DNS_HEADER));
    }
    else {
        // Setting up the DNS query to send.
        qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];
        host_to_dns_format(qname , host);
        qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];
        qinfo->qtype = htons( query_type );   
        qinfo->qclass = htons(1);           
 
        // Sending the query to the DNS server.
        packet_size = sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION);
        send_udp_packet(&s,(char*)buf,packet_size,(struct sockaddr*)&dest,sizeof(dest));

        // Receiving the reponse from the DNS server.
        i = sizeof(dest);
        if(recvfrom (s,(char*)buf , 512 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
            return;
 
        dns = (struct DNS_HEADER*) buf;
        ngethostbyname(buf,dns);
    }

    // Copying the client's ID to the beginning of the buffer (where the DNS packet starts).
    // We do this because we have received the response from another DNS server so,
    // the ID on the packet is the ID that we just used to make the query.
    memcpy(&buf,&q_id,sizeof(unsigned short)); 

    // Set the packet to send back to the client who made the query.
    //packet_size += (ntohs(dns->ans_count) + ntohs(dns->auth_count) + ntohs(dns->add_count))*sizeof(struct R_DATA);    
    packet_size = 512;
    char * packet = malloc(packet_size);
    if(memerror(packet,"new client packet"))
        return;

    memset(packet,0,packet_size);
    memcpy(packet,buf,packet_size);
    send_udp_packet(sock,packet,packet_size,(struct sockaddr *)c_addr,len);
    free(packet);
    printf("Done resolving (%s)\n", host);
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

/**
 * @brief Gets the default DNS servers (works only on linux).
 * @return: Returns (-1) if the function was unable to retrieve the dns servers. 
 * else returns zero.
 */
unsigned short get_dns_servers()
{
    FILE *fp;
    char line[200] , *p;

    printf("Loading resolv.conf file...");

    // Opening the file which contains the DNS servers.
    if((fp = fopen("/etc/resolv.conf" , "r")) == NULL){
        printf("Error : Failed opening /etc/resolv.conf file \n");
        return;
    }

    // We read by 200 chars each time. 
    while(fgets(line , 200 , fp) && dns_servers_count<MAX_DNS_SERVERS-3)
    {
    	// If starts with '#', it's a comment so we skip the line
        if(line[0] == '#')
            continue;
    
        if(strncmp(line , "nameserver" , 10) == 0){
            p = strtok(line , " ");
            p = strtok(NULL , " ");
            p[strlen(p)-1] = '\0';
            strcpy(dns_servers[dns_servers_count],p);
            dns_servers_count++;
        }
    }
    printf( "(DONE)\n");
    return dns_servers_count;
}

/**
 * @brief Converts from the dot format to the number format.
 * @param dns: the output we are going to use for making the query.
 * @param host: the original name of the host (which uses dots).
 */
void host_to_dns_format(unsigned char * dns, unsigned char * host)
{
    unsigned int lock = 0 , i;
    strcat((char*)host,".");
    for(i = 0 ; i < strlen((char*)host) ; i++){
        if(host[i]=='.'){
            *dns++ = i-lock;
            for(;lock<i;lock++)
                *dns++=host[lock];
            lock++;
        }
    }
    *dns++='\0';
}

void ngethostbyname(unsigned char * buf , struct DNS_HEADER * dns)
{
    unsigned char *qname,*reader;
    int i , j , stop , s;
 
    struct sockaddr_in a;
 
    struct RES_RECORD answers[20],auth[20],addit[20]; //the replies from the DNS server
    struct sockaddr_in dest;
 
 
    struct QUESTION *qinfo = NULL;
 
    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];
    host_to_dns_format(qname , host);

    //move ahead of the dns header and the query field
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];
 
    printf("\nThe response contains : ");
    printf("\n %d Questions.",ntohs(dns->q_count));
    printf("\n %d Answers.",ntohs(dns->ans_count));
    printf("\n %d Authoritative Servers.",ntohs(dns->auth_count));
    printf("\n %d Additional records.\n\n",ntohs(dns->add_count));
 
    //Start reading answers
    stop=0;
 
    for(i=0;i<ntohs(dns->ans_count);i++)
    {
        answers[i].name=ReadName(reader,buf,&stop);
        reader = reader + stop;
 
        answers[i].resource = (struct R_DATA*)(reader);
        reader = reader + sizeof(struct R_DATA);
 
        if(ntohs(answers[i].resource->type) == 1) //if its an ipv4 address
        {
            answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));
 
            for(j=0 ; j<ntohs(answers[i].resource->data_len) ; j++)
            {
                answers[i].rdata[j]=reader[j];
            }
 
            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';
 
            reader = reader + ntohs(answers[i].resource->data_len);
        }
        else
        {
            answers[i].rdata = ReadName(reader,buf,&stop);
            reader = reader + stop;
        }
    }
 
    //read authorities
    for(i=0;i<ntohs(dns->auth_count);i++)
    {
        auth[i].name=ReadName(reader,buf,&stop);
        reader+=stop;
 
        auth[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);
 
        auth[i].rdata=ReadName(reader,buf,&stop);
        reader+=stop;
    }
 
    //read additional
    for(i=0;i<ntohs(dns->add_count);i++)
    {
        addit[i].name=ReadName(reader,buf,&stop);
        reader+=stop;
 
        addit[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);
 
        if(ntohs(addit[i].resource->type)==1)
        {
            addit[i].rdata = (unsigned char*)malloc(ntohs(addit[i].resource->data_len));
            for(j=0;j<ntohs(addit[i].resource->data_len);j++)
            addit[i].rdata[j]=reader[j];
 
            addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
            reader+=ntohs(addit[i].resource->data_len);
        }
        else
        {
            addit[i].rdata=ReadName(reader,buf,&stop);
            reader+=stop;
        }
    }
 
    //print answers
    printf("\nAnswer Records : %d \n" , ntohs(dns->ans_count) );
    for(i=0 ; i < ntohs(dns->ans_count) ; i++)
    {
        printf("Name : %s ",answers[i].name);
 
        if( ntohs(answers[i].resource->type) == T_A) //IPv4 address
        {
            long *p;
            p=(long*)answers[i].rdata;
            a.sin_addr.s_addr=(*p); //working without ntohl
            printf("has IPv4 address : %s",inet_ntoa(a.sin_addr));
        }
         
        if(ntohs(answers[i].resource->type)==5)
        {
            //Canonical name for an alias
            printf("has alias name : %s",answers[i].rdata);
        }
 
        printf("\n");
    }
 
    //print authorities
    printf("\nAuthoritive Records : %d \n" , ntohs(dns->auth_count) );
    for( i=0 ; i < ntohs(dns->auth_count) ; i++)
    {
         
        printf("Name : %s ",auth[i].name);
        if(ntohs(auth[i].resource->type)==2)
        {
            printf("has nameserver : %s",auth[i].rdata);
        }
        printf("\n");
    }
 
    //print additional resource records
    printf("\nAdditional Records : %d \n" , ntohs(dns->add_count) );
    for(i=0; i < ntohs(dns->add_count) ; i++)
    {
        printf("Name : %s ",addit[i].name);
        if(ntohs(addit[i].resource->type)==1)
        {
            long *p;
            p=(long*)addit[i].rdata;
            a.sin_addr.s_addr=(*p);
            printf("has IPv4 address : %s",inet_ntoa(a.sin_addr));
        }
        printf("\n");
    }
    return;
}

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

void read_properties_file(const char * fname){

    FILE * prop_file = fopen(fname,"r");
    char buf[256];
    char * line = NULL;
    size_t len;
    prop_type type = NONE;

    printf( "Loading properties file...");
    
    if(!prop_file){
        printf( "(FAILED)\n\tError : Properties file ('%s') could not be loaded.\n",fname);
        return;
    }

    while (fgets(buf, 256, prop_file) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';
        if(buf[0]!='#'){
            if(strcmp(buf,"[blocklist]")==0)
                type = BLOCK_ITEM;
            else if(strcmp(buf,"[default_dns_servers]")==0)
                type = SERVER_ITEM;
            else {
                if(type==NONE){
                    printf( "(FAILED)\n\tError: Properties file ('%s') is invalid.\n",fname);
                    return;
                }
                else if(strcmp(buf,"\n")!=0){
                    if(type==BLOCK_ITEM)
                        blocked_domains_tree = tree_insert_node(blocked_domains_tree,buf,NULL,NULL);
                    else
                        strcpy(dns_servers[dns_servers_count++],buf);    
                }
            }
        }
    }
    printf( "(DONE)\n");
}