//List of DNS Servers registered on the system
extern char dns_servers[10][100];
extern int dns_server_count;

//Function Prototypes
void ngethostbyname(unsigned char *host , int query_type, int * sock, struct sockaddr_in * c_addr,int len,unsigned short q_id);
void ChangetoDnsNameFormat (unsigned char*,unsigned char*);
unsigned char* ReadName (unsigned char*,unsigned char*,int*);
void get_dns_servers();
void * handle_query(void * thread_parameter);
void print_type(unsigned short); 
u_char * get_query_domain(unsigned char * reader,unsigned char * buffer);