#include<unistd.h>

#include "server_functions.h"

int datastore[NUMKEYS];

void idle(int time){
    sleep(time);
}

int get(int key){
    if(key > NUMKEYS || key < 0){
        return -1;
    }
    return datastore[key];
}

int put(int key, int value){
    if(key > NUMKEYS || key < 0){
        return -1;
    }
    datastore[key] = value;
    return 0;
}