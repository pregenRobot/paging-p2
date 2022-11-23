#include "paging.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h> 

void* create_shared_memory(size_t size){
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(){
    void *store = malloc(1<<5);
    void *table = pt_init();

    void* shm_table = create_shared_memory(sizeof(uint32_t) * PAGETABLE_ROWS);
    void* shm_store = create_shared_memory(1<<5);

    memcpy(shm_table, table, sizeof(uint32_t) * PAGETABLE_ROWS);
    memcpy(shm_store, store, 1<<5);
    
    for(int i = 0; i < 3; i++){
        pid_t pid = fork();

        if(pid == 0){// child
            map_page_to_frame(shm_table, 0, 5*i + 4, false, false);
            map_page_to_frame(shm_table, 1, 5*i + 3, false, false);
            map_page_to_frame(shm_table, 2, 5*i + 0, false, false);
            map_page_to_frame(shm_table, 3, 5*i + 2, false, false);
            map_page_to_frame(shm_table, 4, 5*i + 1, false, false);

            char text[640];
            sprintf(text, "THIS IS A MESSAGE FROM CHILD #%d WITH PID %d. Contrary to popular belief, Lorem Ipsum is not simply random text. It has roots in a piece of classical Latin literature from 45 BC, making it over 2000 years old. Richard McClintock, a Latin professor at Hampden-Sydney College in Virginia, looked up one of the more obscure Latin words, consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, discovered the undoubtable source.", i , (int)getpid());
            size_t length = strlen(text);
            store_data(shm_table, shm_store, text, 0, length);
            sleep (5); // wait for all the children to finish writing;
            char* buffer = malloc(length + 1);
            read_data(shm_table, shm_store, buffer, 0, length);
            printf("Child is reading message: %s\n", buffer);
            free(buffer);
            exit(0);
        }
        // parent moves on to next loop
    }

    // Wait for children to finish
    sleep(10);

    printf("PAGE TABLE AFTER all allocation: \n");
    print_table(shm_table);

}
