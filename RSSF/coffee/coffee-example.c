
#include "contiki.h"  
#include <stdio.h>  
#include "sys/log.h"   
#include "os/storage/cfs/cfs.h"  
#define arquivo "teste.txt"
PROCESS(coffee_test_process, "Coffee test process");
 AUTOSTART_PROCESSES(&coffee_test_process);
 
 PROCESS_THREAD(coffee_test_process, ev, data)
 {
   PROCESS_BEGIN();
     
   /* step 1 */
    char message[32];
    char buf[100];   
    // struct cfs_dir *dirp = NULL ;  
    // strcpy(dirp->state,endereco_teste); 
        
    strcpy(message,"#1.hello world.");
    strcpy(buf,message); 

    printf("step 1: %s\n", buf );  
   // int dir_write = cfs_opendir(dirp, arquivo); 
   // if(dir_write != -1 ) printf("Open dir\n");
    int fd_write = cfs_open(arquivo, CFS_WRITE | CFS_APPEND);
    if(fd_write != -1) {
        int n = cfs_write(fd_write, message, sizeof(message));
        cfs_close(fd_write); 
        if(n)   
            printf("step 2: successfully written to cfs. wrote %i bytes\n", n);
    }   

    else {
        printf("ERROR: could not write to memory in step 2.\n");
    }
     
        
   PROCESS_END();
 }