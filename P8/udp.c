#include <stdio.h>	// printf
#include <string.h> // memset
#include <stdlib.h> // exit(0);
#include <unistd.h> // close
#include <sys/time.h> //timeval

#include "udp.h"

// NO CHANGES NEEDED
// READ TO UNDERSTAND HOW TO RECEIVE PACKAGE
// BOTH CLIENT AND CALLER WILL USE THIS CODE
void die(char *s)
{
	perror(s);
	exit(1);
}

struct socket init_socket(int port){
    struct socket my_socket;

	//create a UDP socket
	if ((my_socket.fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
	
	// zero out the structure
	memset((char *) &(my_socket.si), 0, sizeof(my_socket.si));
	
	my_socket.si.sin_family = AF_INET;
	my_socket.si.sin_port = htons(port);
	my_socket.si.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//bind socket to port
	if( bind(my_socket.fd, (struct sockaddr*)&(my_socket.si), sizeof(my_socket.si) ) == -1)
	{
		die("bind");
	}

    return my_socket;
}

struct packet_info receive_packet(struct socket s){
    return receive_packet_timeout(s, 0);
}

struct packet_info receive_packet_timeout(struct socket s, int timeout){
    struct packet_info packet;
    packet.slen = sizeof(packet.sock);

    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    if (setsockopt(s.fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error");
    }

    packet.recv_len = recvfrom(s.fd, packet.buf, BUFLEN, 0, (struct sockaddr *) &(packet.sock), &(packet.slen));
    return packet;

}

void send_packet(struct socket source, struct sockaddr target, int slen, char* payload, int payload_length){
    if (sendto(source.fd, payload, payload_length, 0, (struct sockaddr*) &target, slen) == -1)
    {
        die("send");
    }
}

// source: https://stackoverflow.com/questions/48328708/c-create-a-sockaddr-struct
void populate_sockaddr(int af, int port, char addr[],
        struct sockaddr_storage *dst, socklen_t *addrlen) {

    if (af == AF_INET) {
        // Structures of type sockaddr_in are intended to store the address of the server
        struct sockaddr_in *dst_in4 = (struct sockaddr_in *) dst;

        *addrlen = sizeof(*dst_in4);
        memset(dst_in4, 0, *addrlen);
        dst_in4->sin_family = af;
        dst_in4->sin_port = htons(port);
        inet_pton(af, addr, &dst_in4->sin_addr);

    } else if (af == AF_INET6) {
        struct sockaddr_in6 *dst_in6 = (struct sockaddr_in6 *) dst;

        *addrlen = sizeof(*dst_in6);
        memset(dst_in6, 0, *addrlen);
        dst_in6->sin6_family = af;
        dst_in6->sin6_port = htons(port);
        // unnecessary because of the memset(): dst_in6->sin6_flowinfo = 0;
        inet_pton(af, addr, &dst_in6->sin6_addr);
    }
}

void close_socket(struct socket s){
    close(s.fd);
}
