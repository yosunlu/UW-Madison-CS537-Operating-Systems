#ifndef UDP_H
#define UDP_H

#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFLEN 1024	//Max length of buffer

struct socket{
    struct sockaddr_in si;
    int fd;
};

struct packet_info{
    struct sockaddr sock;
    int recv_len;
    unsigned int slen;
    char buf[BUFLEN];
};

void die(char *s);
struct socket init_socket(int port);
struct packet_info receive_packet(struct socket s);
struct packet_info receive_packet_timeout(struct socket s, int timeout);
void send_packet(struct socket source, struct sockaddr target, int slen, char* payload, int payload_length);
void populate_sockaddr(int af, int port, char addr[], struct sockaddr_storage *dst, socklen_t *addrlen);
void close_socket(struct socket s);

#endif