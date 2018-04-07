#include <stdio.h>
#include <string.h> /* memset() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
 
#define PORT    "32001" /* Port to listen on */
#define BACKLOG     10  /* Passed to listen() */
 
void handle(int newsock, fd_set *set)
{
    /* send(), recv(), close() */
    /* Call FD_CLR(newsock, set) on disconnection */
}
 
int main(void)
{
    int sock;
    fd_set socks;
    fd_set readsocks;
    int maxsock;
    int reuseaddr = 1; /* True */
    struct addrinfo hints, *res;
 
    /* Get the address info */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }
 
    /* Create the socket */
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1) {
        perror("socket");
        return 1;
    }
 
    /* Enable the socket to reuse the address */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) {
        perror("setsockopt");
        return 1;
    }
 
    /* Bind to the address */
    if (bind(sock, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        return 1;
    }
 
    freeaddrinfo(res);
 
    /* Listen */
    if (listen(sock, BACKLOG) == -1) {
        perror("listen");
        return 1;
    }
 
    /* Set up the fd_set */
    FD_ZERO(&socks);
    FD_SET(sock, &socks);
    maxsock = sock;
 
    /* Main loop */
    while (1) {
        unsigned int s;
        readsocks = socks;
        if (select(maxsock + 1, &readsocks, NULL, NULL, NULL) == -1) {
            perror("select");
            return 1;
        }
        for (s = 0; s <= maxsock; s++) {
            if (FD_ISSET(s, &readsocks)) {
                printf("socket %d was ready\n", s);
                if (s == sock) {
                    /* New connection */
                    int newsock;
                    struct sockaddr_in their_addr;
                    socklen_t size = sizeof(struct sockaddr_in);
                    newsock = accept(sock, (struct sockaddr*)&their_addr, &size);
                    if (newsock == -1) {
                        perror("accept");
                    }
                    else {
                        printf("Got a connection from %s on port %d\n", 
                                inet_ntoa(their_addr.sin_addr), htons(their_addr.sin_port));
                        FD_SET(newsock, &socks);
                        if (newsock > maxsock) {
                            maxsock = newsock;
                        }
                    }
                }
                else {
                    /* Handle read or disconnection */
                    handle(s, &socks);
                }
            }
        }
    }
 
    close(sock);
 
    return 0;
}