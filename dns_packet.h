/*
===========================================
=                                         =
=                 Ano-DNS                 =
=               version 0.1               =
=                                         =
=    Developed by Christos Petropoulos    =
=                                         =
===========================================

This header file is used to represent the
structures used to store a DNS packet
in the memory. 

A typical DNS packet is consisted of those main parts : 
    1) The DNS header
    2) A DNS question
    3) A DNS answer
    4) Authority
    5) Some additional fields

The structure of the DNS header includes useful 
information such as what is the id the client used for
the query (you need to always respond including that id
or else the DNS response will be ignored). The DNS header
carries a lot of flags and parameters as can be seen below.
For more info, look for any documentation about the DNS protocol. */

struct DNS_HEADER
{
    /* Variable */              /* Description */

    unsigned short id;          // identification number
 
    // Flags
    unsigned char rd :1;        // recursion desired
    unsigned char tc :1;        // truncated message
    unsigned char aa :1;        // authoritive answer
    unsigned char opcode :4;    // kind of the query
    unsigned char qr :1;        // type of message (query/response)
 
    unsigned char rcode :4;     // response code
    unsigned char cd :1;        // checking disabled
    unsigned char ad :1;        // authenticated data
    unsigned char z :1;         // reserved for future use
    unsigned char ra :1;        // recursion available
 
    // Counters
    unsigned short q_count;     // number of question entries
    unsigned short ans_count;   // number of answer entries
    unsigned short auth_count;  // number of authority entries
    unsigned short add_count;   // number of resource entries
};
 

/*  The structure of a DNS question.
    qtype:  Specifies the type of the query 
    qclass: Specifies the class of the query */
struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};
 
/*  The structure of a resource record.
    type: Specifies the meaning of the data (rdata)
    class: Specifies the class of the data (rdata)
    ttl: Specifies the number of seconds the results can be cached
    data_len: Specifies the length of the data  */

/* Types of a DNS resource record */
 
#define T_A 1       // ipv4 address
#define T_NS 2      // nameserver
#define T_CNAME 5   // canonical name
#define T_SOA 6     // start of authority zone 
#define T_PTR 12    // domain name pointer 
#define T_MX 15     // mail server

struct R_DATA
{
    unsigned short  type;
    unsigned short  _class;
    unsigned int    ttl;
    unsigned short  data_len;
};

/*  A structure which contains pointers to
    the info of a resource record. */
struct RES_RECORD
{
    unsigned char * name;
    struct R_DATA * resource;
    unsigned char * rdata;
};

/* The basic structure of a query */ 
typedef struct
{
    unsigned char * name;
    struct QUESTION * ques;
} QUERY;