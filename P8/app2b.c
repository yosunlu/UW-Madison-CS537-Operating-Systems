#include <stdio.h>

#include "client.h"

int main(){
    struct rpc_connection rpc = RPC_init(1238, 7892, "127.0.0.1");

    RPC_idle(&rpc, 5);
    RPC_idle(&rpc, 5);
    printf("Found value: %d\n", RPC_get(&rpc, 10));

    RPC_close(&rpc);
}
