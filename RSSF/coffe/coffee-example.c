
#include "contiki.h"  
#include <stdio.h>  
#include "sys/log.h"   
 #include "cfs/cfs.h"

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
   fd_write = cfs_open(filename, CFS_WRITE);
   if(fd_write != -1) {
   n = cfs_write(fd_write, message, sizeof(message));
   cfs_close(fd_write);
   printf("step 2: successfully written to cfs. wrote %i bytes\n", n);
   }  
   else {
   printf("ERROR: could not write to memory in step 2.\n");
    }
     
        
   PROCESS_END();
 }