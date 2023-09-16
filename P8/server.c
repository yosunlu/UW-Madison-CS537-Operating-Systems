#include "server_functions.h"
#include "udp.h"
#include "payload.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>



typedef struct _thread_data_t {
  int* attr;
  struct st_payload* payloadTable;
  struct packet_info* myPackInfo;
  struct socket mySocket;
  int numClient;
  int count;

} thread_data_t;


void threadTasks(int* attr, struct st_payload * payloadTable, struct packet_info* myPackInfo, struct socket mySocket, int numClient, int count){
    int retVal = 0;

    switch(attr[0]){
        case 1: // idle
            // printf("sleep1\n");
            // idle(5);
            // printf("sleep2\n");
            // pthread_t thr_idle;
            idle(attr[3]);
            break;

        case 2: // get
            // void send_packet(struct socket source, struct sockaddr target, int slen, char* payload, int payload_length){
            retVal = get(attr[4]);
            break;
        
        case 3:
            put(attr[4], attr[5]);
            break;
    }

    // //if it's a new request, store the return value to the st_payload in payloadTable based on index "count"
    // if(numClient == -1){
    //     payloadTable[attr[1]].returnValue = retVal;
    //     //printf("retuenVal: %d\n\n", retVal);
    // }
    //if it's an old request
    // else
    if(attr[0] == 2)
        payloadTable[attr[1]].returnValue = retVal;
    
    //set ack back to 0
    int ack = 0;
    int payload_length = 32;
    memcpy((char*)(myPackInfo->buf + 24), &ack, 4);
    memcpy((char*)(myPackInfo->buf + 28), &retVal, sizeof(retVal));

     

    send_packet(mySocket, myPackInfo->sock, myPackInfo->slen, myPackInfo->buf, payload_length);
}

void* threadFunc(void* arg){
  thread_data_t *data = arg;
  printf("server key: %d\n", data->attr[4]);
    printf("server value: %d\n\n", data->attr[5]);
    printf("server client id: %d\n", data->attr[1]);
  threadTasks(data->attr, data->payloadTable, data->myPackInfo, data->mySocket, data->numClient, data->count);
  pthread_exit(NULL);
}


int main(int argc, char** argv){
    
    struct st_payload payloadTable[100];

    for(int i = 0; i < 100; i++){
        payloadTable[i].clientId = -1;
    }
    int count = 0;

    int port = atoi(argv[1]);
    struct socket mySocket = init_socket(port);

    int ack;

    while(1){

        struct packet_info* myPackInfo = (struct packet_info*)malloc(sizeof(struct packet_info));

        *myPackInfo = receive_packet(mySocket);
        
        // 0: type; 1: clientId; 2: seqNumber; 3: time; 4: key; 5: value; 6: ack; 7: returnValue
        int* attr = malloc(32);

        int j = 0;
        for(int i = 0; i < 32; i += 4){
            memcpy(attr+j,(int*)(myPackInfo->buf+i),4);
              
            j++; // ???????????????????
        }
        // printf("myPackInfo.buf0 type: %d\n", *(int*)myPackInfo.buf);
        // printf("myPackInfo.buf1 clientID: %d\n", *(int*)(myPackInfo.buf+4));
        // printf("myPackInfo.buf2 seqNumber: %d\n", *(int*)(myPackInfo.buf+8));
        // printf("myPackInfo.buf3 time: %d\n", *(int*)(myPackInfo.buf+12));
        // printf("myPackInfo.buf4 key: %d\n", *(int*)(myPackInfo.buf+16));
        // printf("myPackInfo.buf5 value: %d\n", *(int*)(myPackInfo.buf+20));
        // printf("myPackInfo.buf6 ack: %d\n", *(int*)(myPackInfo.buf+24));
        // printf("myPackInfo.buf7 returnvalue: %d\n", *(int*)(myPackInfo.buf+28));

        // printf("attr0 type: %d\n", attr[0]);
        // printf("attr1 clientID: %d\n", attr[1]);
        // printf("attr2 seqNumber: %d\n", attr[2]);
        // printf("attr3 time: %d\n", attr[3]);
        // printf("attr4 key: %d\n", attr[4]);
        // printf("attr5 value: %d\n", attr[5]);
        // printf("attr6 ack: %d\n", attr[6]);
        // printf("attr7 returnvalue: %d\n", attr[7]);

        int numClient = -1;

        //numClient = attr[1];
        
        
        for(int i = 0; i < 100; i++){
            //printf("attr[1]: %d\n", attr[1]);
            if(payloadTable[i].clientId == attr[1]){
        //        printf("payloadTable[i].clientId: %d\n", payloadTable[i].clientId);
        //        //printf("attr[1]: %d\n", attr[1]);
        //        printf("myPackInfo.buf0 type: %d\n", *(int*)myPackInfo.buf);
        //        printf("myPackInfo.buf1 clientID: %d\n", *(int*)(myPackInfo.buf+4));
        //printf("myPackInfo.buf2 seqNumber: %d\n", *(int*)(myPackInfo.buf+8));
        //printf("myPackInfo.buf3 time: %d\n", *(int*)(myPackInfo.buf+12));
        //printf("myPackInfo.buf4 key: %d\n", *(int*)(myPackInfo.buf+16));
        //printf("myPackInfo.buf5 value: %d\n", *(int*)(myPackInfo.buf+20));
        //printf("myPackInfo.buf6 ack: %d\n", *(int*)(myPackInfo.buf+24));
        //printf("myPackInfo.buf7 returnvalue: %d\n", *(int*)(myPackInfo.buf+28));
                numClient = i;
                break;
            }                
        }
        
        printf("payloadTable[count].funcType: %d\n", attr[0]);
        printf("payloadTable[count].clientId: %d\n", attr[1]);
        printf("payloadTable[count].seqNumber: %d\n", attr[2]);
        printf("payloadTable[count].time: %d\n", attr[3]);
        printf("payloadTable[count].key: %d\n", attr[4]);
        printf("payloadTable[count].value: %d\n", attr[5]);
        printf("payloadTable[count].ack: %d\n", attr[6]);
        printf("payloadTable[count].returnValue: %d\n\n\n", attr[7]);
        //if it's a new request
        if(numClient == -1){
            
            payloadTable[attr[1]].funcType = attr[0];
            payloadTable[attr[1]].clientId = attr[1];
            payloadTable[attr[1]].seqNumber = 0;
            payloadTable[attr[1]].time = attr[3];
            payloadTable[attr[1]].key = attr[4];
            payloadTable[attr[1]].value = attr[5];
            payloadTable[attr[1]].ack = attr[6];
            payloadTable[attr[1]].returnValue = attr[7];

            
            //payloadTable[attr[1]].hasRequested = 1;
            //count++;
            
            
        }
        //the client has requested
        // else{
            //if the sequence number of the current request is larger than the sequence number of the older request
            if(payloadTable[attr[1]].seqNumber < attr[2]){
                // printf("LARGER payloadTable[count].funcType: %d\n", attr[0]);
                // printf("LARGER payloadTable[count].clientId: %d\n", attr[1]);
                //increment the sequence number of the older request
                //so when the same request is requested, ack will be sent back to the client
                // payloadTable[numClient].seqNumber++;

                payloadTable[attr[1]].seqNumber = attr[2];
                printf("payloadTable[count].seqNumber: %d\n\n", payloadTable[attr[1]].seqNumber);
            }
            //if the sequence number of the current request is the same as the older request
            else if(payloadTable[attr[1]].seqNumber == attr[2]){
                
                //send ack
                // printf("send ack\n\n\n\n\n");
                // if(payloadTable[numClient].funcType == 1){
                    // printf("EQUAL payloadTable[count].funcType: %d\n", attr[0]);
                    // printf("EQUAL payloadTable[count].clientId: %d\n", attr[1]);
                    printf("EQUAL payloadTable[count].seqNumber: %d\n\n", payloadTable[attr[1]].seqNumber);
                    int retVal = payloadTable[attr[1]].returnValue;
                    printf("return value: %d\n\n", payloadTable[attr[1]].returnValue);
                    ack = 1;
                    memcpy((char*)(myPackInfo->buf + 24), &ack, 4);
                    memcpy((char*)(myPackInfo->buf + 28), &retVal, 4);
                // }
                // else{
                //     // printf("2 EQUAL payloadTable[count].funcType: %d\n", attr[0]);
                //     // printf("2 EQUAL payloadTable[count].clientId: %d\n", attr[1]);
                //     printf("payloadTable[count].seqNumber: %d\n\n", payloadTable[attr[1]].seqNumber);

                //     int oldVal;
                //     oldVal = payloadTable[numClient].returnValue;
                //     memcpy((char*)(myPackInfo.buf + 28), &oldVal, 4);
                // }
                send_packet(mySocket, myPackInfo->sock, myPackInfo->slen, myPackInfo->buf, 32);
                continue;
            }
            else{
            printf("DISCARD payloadTable[count].funcType: %d\n", attr[0]);
            printf("DISCARD payloadTable[count].clientId: %d\n\n", attr[1]);
            //discard
                continue;
            }
        // }
        
        // printf("number of client: %d\n", count);
        pthread_t thr;
        // struct _thread_data_t data = {attr, payloadTable, myPackInfo, mySocket, numClient, count}; // struct for Func's arguments
        struct _thread_data_t *data = (struct _thread_data_t*)malloc(sizeof(struct _thread_data_t));
        data->attr = attr;
        data->payloadTable = payloadTable;
        data->myPackInfo = myPackInfo;
        data->mySocket = mySocket;
        data->numClient = numClient;
        data->count = count;
        printf(" key: %d\n", data->attr[4]);
        printf(" value: %d\n", data->attr[5]);
        printf(" client id: %d\n\n", data->attr[1]);
        pthread_create(&thr, NULL, threadFunc, data);
        // pthread_join(thr, NULL);
        pthread_detach(thr);
        
    }
    close_socket(mySocket);
    return 0;
}
