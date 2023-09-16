#include "types.h"
#include "stat.h"
#include "user.h"

// Test that multiple copies are created when pages shared by more processes are modified

void modify_array(int* arr, int sz){
    for(int i = 0; i < sz; i++){
        arr[i] += arr[i];
    }
}

int main(void) {
    int sz = 1000, sz2 = 10001;
    int* arr = (int*)malloc(sz * sizeof(int)); // occupies 1 page
    int* arr2 = (int*)malloc(sz2 * sizeof(int)); // occupies 10 pages

    for(int i = 0; i < sz; i++){
        arr[i] = i;
    }
    for(int i = 0; i < sz2; i++){
        arr2[i] = i;
    }

    // int init_free = getFreePagesCount();
    // printf(1, "Initial: Free=%d\n", init_free);

    int free_after_forks, final_free;

    // Process structure: main --> child1 --> child2
    
    int pid = fork();
    if(pid == 0){ // child1
        int pid2 = fork();

        if(pid2 == 0) {
            free_after_forks = getFreePagesCount();
            // printf(1, "Free pages after fork: %d\n", free_after_forks);
        } else {
            sleep(20); // sufficient wait to ensure free_after_forks is not after arrays are modified
        }

        // both parent and child modify the arrays -- + (2 x 11) pages
        modify_array(arr, sz);
        modify_array(arr2, sz2);

        if(pid2 == 0) { // child
            sleep(100); // sufficient wait for parent to finish modifying arrays

            final_free = getFreePagesCount();
            printf(1, "final_free (Free pages after writes): %d\n", final_free);
            printf(1, "free_after_forks: %d\n", free_after_forks);
            // printf(1, "Free pages after writes: %d\n", final_free);

            if(free_after_forks != final_free + (2 * 11)){
                printf(1, "XV6_COW\t FAILED\n");
            } else {
                printf(1, "XV6_COW\t SUCCESS\n");
            }
        } else {
            wait();
        }
    } else {
        wait();
    }

    exit();
}
