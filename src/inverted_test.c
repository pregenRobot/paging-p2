#include "paging.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

void* create_shared_memory(size_t size){
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(){
    void *store = malloc(1<<5);
    void *table = pt_init();

    void* shm_store = create_shared_memory(sizeof(store));
    void* shm_table = create_shared_memory(sizeof(table));

    memcpy(shm_store, store, sizeof(store));
    memcpy(shm_table, table, sizeof(table));

    for(int i = 0; i < 3; i++){
        pid_t pid = fork();

        if(pid == 0){// child
            map_page_to_frame(table, 0, 3*i + 4, false, false);
            map_page_to_frame(table, 0, 3*i + 4, false, false);
            map_page_to_frame(table, 0, 3*i + 0, false, false);
            map_page_to_frame(table, 0, 3*i + 2, false, false);
            map_page_to_frame(table, 0, 3*i + 1, false, false);

            char text[128];
            sprintf(text, "This is a message from child %d with pid %d", i , pid);
            size_t length = strlen(text);
            store_data(shm_table, shm_store, text, 0, length);
            sleep (5); // wait for all the children to finish writing;
            char* buffer = malloc(length + 1);
            read_data(shm_table, shm_store, buffer, 0, length);
            printf("Child is reading message: %s\n", buffer);
            free(buffer);
        }
        // parent moves on to next loop
    }

    // Wait for children to finish
    sleep(10);

}
