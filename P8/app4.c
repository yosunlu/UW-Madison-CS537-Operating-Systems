// Test parallel client handling.
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <pthread.h>
#include "client.h"

void *sendPut(void *vargp)
{
    int* client_id = (int *) vargp;
    printf("client id %d\n", *client_id);
    struct rpc_connection rpc = RPC_init(*client_id + 8000, 7894, "127.0.0.1");
    rpc.client_id = *client_id;
    RPC_put(&rpc, *client_id, 1234);
    sleep(1);
    printf("put get value: %d\n", RPC_get(&rpc, *client_id));

    RPC_close(&rpc);

    return NULL;
}



int main(){
    int num_threads = 10;
    pthread_t thread_id[num_threads];
    for(int i = 0; i < num_threads; i++) {
        int *a = (int*) malloc(sizeof(int));
        *a = i;
        pthread_create(&thread_id[i], NULL, sendPut, a);
    }

    for(int j = 0; j < num_threads; j++) {
        pthread_join(thread_id[j], NULL);
    }
}

