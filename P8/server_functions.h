#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H
// BUILDING THE PERSON BETWEEN UDP AND ??
#define NUMKEYS 1024

// Sleeps the thread for a give amount of seconds seconds
void idle(int time);

// gets the value of a key on the server store
int get(int key);

// sets the value of a key on the server store
int put(int key, int value);

#endif