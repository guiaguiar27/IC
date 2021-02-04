
#include "contiki.h"  
#include <stdio.h>  
#include "sys/log.h"   
 #include "os/storage/cfs/cfs.h" 
 #define endereco_teste  "/home/user/contiki-ng/os/teste.txt"

PROCESS(coffee_test_process, "Coffee test process");
 AUTOSTART_PROCESSES(&coffee_test_process);
 
 PROCESS_THREAD(coffee_test_process, ev, data)
 {
   PROCESS_BEGIN();
     
   /* step 1 */
   char message[32];
   char buf[100];
     
   strcpy(message,"#1.hello world.");
   strcpy(buf,message); 

   printf("step 1: %s\n", buf ); 
   int fd_write = cfs_open(endereco_teste, CFS_WRITE | CFS_APPEND);
   if(fd_write != -1) {
   int n = cfs_write(fd_write, message, sizeof(message));
   cfs_close(fd_write);
   printf("step 2: successfully written to cfs. wrote %i bytes\n", n);
   }  
   else {
   printf("ERROR: could not write to memory in step 2.\n");
    }
     
        
   PROCESS_END();
 }