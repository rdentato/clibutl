/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                        ___   __
**                     __/  /_ /  )
**             ___  __(_   ___)  /
**            /  / /  )/  /  /  /
**           /  (_/  //  (__/  / 
**          (____,__/(_____(__/
**    https://github.com/rdentato/clibutl
** 
*/


//<<<//
#ifdef UTL_NET
 

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#ifndef __GNUC__
#pragma comment(lib, "Ws2_32.lib")
#endif
#else

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#endif

int utl_net_listen(uint32_t addr, int port, int (*hnd)(char *,int), long timeout, int (*tmout)(void)) ;
int utl_net_nohandle(char *msg,int len);
int utl_net_notimeout(void);


#define utlserver(...)       utl_net_listen utl_expand ((INADDR_ANY, \
                                                  utl_arg0(__VA_ARGS__,0,0,0,0), \
                                                  utl_arg1(__VA_ARGS__,utl_net_nohandle,utl_net_nohandle,utl_net_nohandle,utl_net_nohandle), \
                                                  utl_arg2(__VA_ARGS__,0,0,0,0), \
                                                  utl_arg3(__VA_ARGS__,utl_net_notimeout,utl_net_notimeout,utl_net_notimeout,utl_net_notimeout) ))

#define utlserverlocal(...)  utl_net_listen utl_expand ((INADDR_LOOPBACK, \
                                                  utl_arg0(__VA_ARGS__,0,0,0,0), \
                                                  utl_arg1(__VA_ARGS__,utl_net_nohandle,utl_net_nohandle,utl_net_nohandle,utl_net_nohandle), \
                                                  utl_arg2(__VA_ARGS__,0,0,0,0), \
                                                  utl_arg3(__VA_ARGS__,utl_net_notimeout,utl_net_notimeout,utl_net_notimeout,utl_net_notimeout) ))

#endif
//>>>//
