/*
* * * * * * * * * * * * * * * * * * * * * *
*                                         *
*                 Ano-DNS                 *
*               version 0.1               *
*                                         *
*    Developed by Christos Petropoulos    *
*                                         *
* * * * * * * * * * * * * * * * * * * * * *

This header file includes some system libraries
It's all collected here so no big space just for
including libraries.
*/

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ifaddrs.h> 
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <stddef.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>