#include <stdio.h>

#include "client.h"

int main(){
    struct rpc_connection rpc = RPC_init(1236, 8888, "127.0.0.1");

    RPC_put(&rpc, 1, 1234);

    int value = RPC_get(&rpc, 1);
    printf("put get: %d\n", value);

    rpc.seq_number--;

    value = RPC_get(&rpc, 1);
    printf("put get: %d\n", value);

    RPC_close(&rpc);
}
