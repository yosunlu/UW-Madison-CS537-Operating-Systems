#include "client.h"
#include "udp.h"
#include "payload.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>


struct rpc_connection RPC_init(int src_port, int dst_port, char dst_addr[]){
    
    struct socket mySocket = init_socket(src_port);

    struct sockaddr_storage addr;
    socklen_t addrlen;
    struct timeval time; 
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
    // int rawClientId = rand() % 100;
    // printf("client ID in client.c: %d\n", clientId);
    int clientId = rand() % 100;

    populate_sockaddr(AF_INET, dst_port, dst_addr, &addr, &addrlen);    
    struct rpc_connection rpcReturn = {mySocket, *((struct sockaddr *)(&addr)), addrlen, 1, clientId};
    
    return rpcReturn;
}

// Sleeps the server thread for a few seconds
void RPC_idle(struct rpc_connection *rpc, int time){

   
    struct st_payload myPayload;
    myPayload.funcType = 1;
    myPayload.time = time;
    myPayload.clientId = rpc->client_id;
    myPayload.seqNumber = rpc->seq_number;
    myPayload.ack = 0;
    myPayload.key = -1;
    myPayload.value = -1;
    myPayload.returnValue = -1;

    
    int payload_length = sizeof(myPayload);
    char stream[payload_length];

    memcpy(stream, &myPayload, payload_length);

//  send packet to the server
    // send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);


//  receive packet from the server
    // struct packet_info receive_packet_timeout(struct socket s, int timeout)
    
    struct packet_info timeOutPacket;
    // printf("idle\n");

    rpc->seq_number++;
    // int isDiscard;
    for(int i = 0; i < 5; i++){
        // isDiscard = 0;
        send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);
        timeOutPacket = receive_packet_timeout(rpc->recv_socket, 1);
        // printf("idle timeOutPacket): %d\n", timeOutPacket.recv_len);
        //if ack is 1, reset i
        if(*(int*)(timeOutPacket.buf+24) == 1){
            i = 0;
            sleep(1);
            continue;
        }

        
        //if the request was discarded on the server
        if(timeOutPacket.recv_len == -1){
            // printf("time out\n\n");
            // send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);
            // isDiscard = 1;
            continue;

        }
        //if return successfully (ack is 0), increment sequence number
        else if(*(int*)(timeOutPacket.buf+24) == 0){
            // rpc->seq_number++;
            break;
        }

        // if(isDiscard == 0)
        //     //the size of timeOutPacket is zero if time out
        //     send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);

        
    }

    // printf("size: %ld\n", sizeof(timeOutPacket));

    // printf("idle myPackInfo.buf0 type: %d\n", *(int*)timeOutPacket.buf);
    // printf("idle myPackInfo.buf1 clientID: %d\n", *(int*)(timeOutPacket.buf+4));
    // printf("idle myPackInfo.buf2 seqNumber: %d\n", *(int*)(timeOutPacket.buf+8));
    // printf("idle myPackInfo.buf3 time: %d\n", *(int*)(timeOutPacket.buf+12));
    // printf("idle myPackInfo.buf4 key: %d\n", *(int*)(timeOutPacket.buf+16));
    // printf("idle myPackInfo.buf5 value: %d\n", *(int*)(timeOutPacket.buf+20));
    // printf("idle myPackInfo.buf6 ack: %d\n", *(int*)(timeOutPacket.buf+24));
    // printf("idle myPackInfo.buf7 returnvalue: %d\n", *(int*)(timeOutPacket.buf+28));
}


// gets the value of a key on the server store
int RPC_get(struct rpc_connection *rpc, int key){

    struct st_payload myPayload;
    myPayload.funcType = 2;
    myPayload.key = key;
    myPayload.clientId = rpc->client_id;
    myPayload.seqNumber = rpc->seq_number;
    myPayload.ack = 0;
    myPayload.time = -1;
    myPayload.returnValue = -1;


    
    int payload_length = sizeof(myPayload);
    char stream[payload_length];

    memcpy(stream, &myPayload, payload_length);

//  send packet to the server
    // send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);

//  receive packet from the server
    struct packet_info timeOutPacket;
    //printf("GET client id: %d\n\n", rpc->client_id);
    rpc->seq_number++;
    for(int i = 0; i < 5; i++){
        send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);
        timeOutPacket = receive_packet_timeout(rpc->recv_socket, 1);
        // printf("idle timeOutPacket): %d\n", timeOutPacket.recv_len);
        //if ack is 1, reset i
        if(*(int*)(timeOutPacket.buf+24) == 1){
            // printf("get 1");
            if(*(int*)(timeOutPacket.buf + 28) != -1)
                return *(int*)(timeOutPacket.buf + 28);

            i = 0;
            sleep(1);
            continue;
        }
        
        //if the request was discarded on the server
        if(timeOutPacket.recv_len == -1){
            // printf("GET time out\n\n");
            // send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);
            // isDiscard = 1;
            continue;

        }
        //if return successfully (ack is 0), increment sequence number
        else if(*(int*)(timeOutPacket.buf+24) == 0){
            // printf("get 2");
            //printf("client %d GET success\n", rpc->client_id);
            // rpc->seq_number++;
            return *(int*)(timeOutPacket.buf + 28);
            // break;
        }
        
    }

    // printf("size: %ld\n", sizeof(timeOutPacket));

    // printf("get myPackInfo.buf0 type: %d\n", *(int*)timeOutPacket.buf);
    // printf("get myPackInfo.buf1 clientID: %d\n", *(int*)(timeOutPacket.buf+4));
    // printf("get myPackInfo.buf2 seqNumber: %d\n", *(int*)(timeOutPacket.buf+8));
    // printf("get myPackInfo.buf3 time: %d\n", *(int*)(timeOutPacket.buf+12));
    // printf("get myPackInfo.buf4 key: %d\n", *(int*)(timeOutPacket.buf+16));
    // printf("get myPackInfo.buf5 value: %d\n", *(int*)(timeOutPacket.buf+20));
    // printf("put myPackInfo.buf6 ack: %d\n", *(int*)(timeOutPacket.buf+24));
    // printf("get myPackInfo.buf7 returnvalue: %d\n", *(int*)(timeOutPacket.buf+28));
    perror("error");
    exit(1);
    return *(int*)(timeOutPacket.buf + 28);
}

// sets the value of a key on the server store
int RPC_put(struct rpc_connection *rpc, int key, int value){
   
    struct st_payload myPayload;
    myPayload.funcType = 3;
    myPayload.clientId = rpc->client_id;
    myPayload.seqNumber = rpc->seq_number;
    myPayload.key = key;
    myPayload.value = value;
    myPayload.ack = 0;
    myPayload.time = -1;
    myPayload.returnValue = -1;

    
    int payload_length = sizeof(myPayload);

    payload_length = sizeof(myPayload);
    char stream[payload_length];

    memcpy(stream, &myPayload, payload_length);
    
//  send packet to the server
    // send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);

    
    struct packet_info timeOutPacket;
    //printf("PUT client id: %d\n", rpc->client_id);

    rpc->seq_number++;
    for(int i = 0; i < 5; i++){
        send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);
        timeOutPacket = receive_packet_timeout(rpc->recv_socket, 1);
        // printf("idle timeOutPacket): %d\n", timeOutPacket.recv_len);
        //if ack is 1, reset i
        if(*(int*)(timeOutPacket.buf+24) == 1){
            return *(int*)(timeOutPacket.buf + 28);
            // i = 0;
            // sleep(1);
            // continue;
        }
        
        //if the request was discarded on the server
        if(timeOutPacket.recv_len == -1){
            printf("PUT time out\n\n");
            // send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, stream, payload_length);
            continue;
            // isDiscard = 1;

        }
        //if return successfully (ack is 0), increment sequence number
        else if(*(int*)(timeOutPacket.buf+24) == 0){
            //printf("client %d PUT success\n", rpc->client_id);
            // rpc->seq_number++;
            //printf("client %d seq num: %d\n", rpc->client_id, rpc->seq_number);
            return *(int*)(timeOutPacket.buf + 28);
        }
    }
    
    // printf("size: %ld\n", sizeof(timeOutPacket));

    // printf("put myPackInfo.buf0 type: %d\n", *(int*)timeOutPacket.buf);
    // printf("put myPackInfo.buf1 clientID: %d\n", *(int*)(timeOutPacket.buf+4));
    // printf("put myPackInfo.buf2 seqNumber: %d\n", *(int*)(timeOutPacket.buf+8));
    // printf("put myPackInfo.buf3 time: %d\n", *(int*)(timeOutPacket.buf+12));
    // printf("put myPackInfo.buf4 key: %d\n", *(int*)(timeOutPacket.buf+16));
    // printf("put myPackInfo.buf5 value: %d\n", *(int*)(timeOutPacket.buf+20));
    // printf("put myPackInfo.buf6 ack: %d\n", *(int*)(timeOutPacket.buf+24));
    // printf("put myPackInfo.buf7 returnvalue: %d\n", *(int*)(timeOutPacket.buf+28));
    perror("error");
    exit(1);
    return *(int*)(timeOutPacket.buf + 28);
}


// closes the RPC connection to the server
void RPC_close(struct rpc_connection *rpc){
    close_socket(rpc->recv_socket);
}
