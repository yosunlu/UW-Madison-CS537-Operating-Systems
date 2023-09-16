// Test duplicate request handling.
#include <stdio.h>

#include "client.h"

int main(){
    struct rpc_connection rpc = RPC_init(1236, 7899, "127.0.0.1");

    RPC_put(&rpc, 1, 1234);

    int value = RPC_get(&rpc, 1);
    printf("get result: %d\n", value);

    rpc.seq_number -= 2;

    value = RPC_get(&rpc, 1);
    printf("get result for older seq no: %d\n", value);

    RPC_close(&rpc);
}
