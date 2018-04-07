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

#include "utl.h"

//<<<//
#ifdef UTL_NET
 
#ifdef _WIN32
#define  cleanup(err) (WSACleanup(),err)
#define  initsock()    WSADATA wsdata; \
                       if (WSAStartup( MAKEWORD(2, 2), &wsdata)) \
                         return WSANOTINITIALISED;
#define  SIZE_T int
#else
#define  SOCKET int
#define  SOCKET_ERROR -1
#define  INVALID_SOCKET -1
#define  closesocket close
#define  cleanup(err) (err)
#define  initsock()  
#define  SIZE_T size_t
#endif
 
#define BACKLOG 10

#define MAXBUF 1024
static char buf[MAXBUF];


int utl_net_nohandle(char *msg,int len) { return len; } // Echo
int utl_net_notimeout(void)             { return 0; }

static int gotbytes(SOCKET sock, int (*hnd)(char *,int) ,fd_set *set)
{
  int n;
  
  n = recv(sock,buf,MAXBUF-1,0);
  buf[n] = '\0';

  //printf("[%s]\n",buf); fflush(stdout);
  if (n > 0) {
    n = hnd(buf,n);
    if (n > 0) {
      send(sock,buf,n,0);
    }
  }
  else {
    FD_CLR(sock, set);
    closesocket(sock);
  }
  return n;
}
 
static SOCKET newconn(SOCKET srv, SOCKET max, fd_set *set)
{
  SOCKET cln;
  struct sockaddr_in client;
  SIZE_T size = sizeof(struct sockaddr_in);

  cln = accept(srv, (struct sockaddr*)&client, &size);
  if (cln == INVALID_SOCKET) {
    // ERRROR TO BE LOGGED
  }
  else {
    //printf("Connection from %s:%d\n", inet_ntoa(client.sin_addr), htons(client.sin_port));
    //fflush(stdout);
    FD_SET(cln, set);
    if (cln > max)  max = cln;
  }
  return max;
}
 
int utl_net_listen(uint32_t addr, int port, int (*hnd)(char *,int), long timeout, int (*tmout)(void)) 
{
  SOCKET srvsock;
  SOCKET maxsock;

  fd_set socksfd;
  fd_set rdsocks;
  struct timeval seltimeout; 
  struct timeval *seltimeoutptr = NULL; 
  struct sockaddr_in srvaddr;
  int    run = 1;
   
  seltimeout.tv_usec = 0;
  if (timeout > 0) {
    seltimeoutptr = &seltimeout;  
  }
  
  initsock();
 
  memset(&srvaddr,0,sizeof(srvaddr));
  srvaddr.sin_family      = AF_INET;
  srvaddr.sin_port        = htons(port);
  srvaddr.sin_addr.s_addr = htonl(addr);
 
  srvsock = socket(AF_INET , SOCK_STREAM, 0);
  if (srvsock == INVALID_SOCKET) {
    return cleanup(errno);
  }
 
  int reuseaddr = 1; 
  if (setsockopt(srvsock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseaddr, 
                                                               sizeof(int)) == SOCKET_ERROR) {
    return cleanup(errno);
  }
 
  if (bind(srvsock, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
    return cleanup(errno);
  }
   
  if (listen(srvsock, BACKLOG) == SOCKET_ERROR) {
    return cleanup(errno);
  }
 
  FD_ZERO(&socksfd);
  FD_SET(srvsock, &socksfd);
  maxsock = srvsock;

  while (run >= 0) {
    rdsocks = socksfd;
    seltimeout.tv_sec = timeout;
    switch (select(maxsock + 1, &rdsocks, NULL, NULL, seltimeoutptr)) {
      case -1: return cleanup(errno);
      
      case  0: //printf("Timeout\n"); fflush(stdout);
               run = tmout();
               break;
               
      default: for (SOCKET s = 0; s <= maxsock; s++) {
                 if (FD_ISSET(s, &rdsocks)) {
                   // printf("Socket %d ready\n", (int)s); fflush(stdout);
                   if (s == srvsock) {
                     maxsock = newconn(srvsock, maxsock, &socksfd);
                   }
                   else {
                     run = gotbytes(s, hnd, &socksfd);
                   }
                 }
               }
    }
  }

  closesocket(srvsock);
  return cleanup(0);
}

#endif
//>>>//
