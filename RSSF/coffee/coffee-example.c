
#include "contiki.h"  
#include <stdio.h>  
#include "sys/log.h"   
#include "os/storage/cfs/cfs.h"   
#include "sys/node-id.h"  
//#define arquivo "teste.txt"
PROCESS(coffee_test_process, "Coffee test process");
 AUTOSTART_PROCESSES(&coffee_test_process);
 
 PROCESS_THREAD(coffee_test_process, ev, data)
 {
   PROCESS_BEGIN();
     
   /* step 1 */   
   char message[3];
   if(node_id == 1 ){ 
     strcpy(message,"1\n");
   } 
   else if(node_id == 2 ){ 
     strcpy(message,"2\n");
   } 
  else if(node_id == 3 ){ 
     strcpy(message,"3\n");
   }   
    
    char buf[100];    
    char *arquivo = " "; 
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
    int fd_read = cfs_open(arquivo, CFS_READ); 
    if(fd_read != -1) {
        cfs_read(fd_read, buf, sizeof(message));
        printf("%s\n", buf); 
    }
    else {
        printf("ERROR: could not write to memory in step 2.\n");
    }
     
        
   PROCESS_END();
 }