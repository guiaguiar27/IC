/*
 * Copyright (c) 2020, Institute of Electronics and Computer Science (EDI)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/**
 * \file
 *         An example that demonstrates a simple custom scheduler for TSCH.
 *
 * \author Atis Elsts <atis.elsts@edi.lv>
 */

#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/mac/tsch/tsch.h"
#include "lib/random.h"
#include "sys/node-id.h" 
#include "sys/energest.h"    
#include "net/ipv6/uip-ds6-nbr.h"  


#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_PORT	8765
#define SEND_INTERVAL		  (60 * CLOCK_SECOND) 


PROCESS(node_process, "TSCH Schedule Node");
AUTOSTART_PROCESSES(&node_process); 
static linkaddr_t coordinator_addr =  {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};

/*
 * Note! This is not an example how to design a *good* schedule for TSCH,
 * nor this is the right place for complete beginners in TSCH.
 * We recommend using the default Orchestra schedule for a start.
 *
 * The intention of this file is instead to serve a starting point for those interested in building
 * their own schedules for TSCH that are different from Orchestra and 6TiSCH minimal.
 */

/* Put all cells on the same slotframe */
#define APP_SLOTFRAME_HANDLE 1
#define APP_UNICAST_TIMESLOT 16 
#define APP_CHANNEL_OFSETT 16   


int  aux_id  ;  


// static unsigned long
// to_seconds(uint64_t time)
// {
//   return (unsigned long)(time / ENERGEST_SECOND);
// }

 
struct latency_structure{ // Structure to save the time when the message was send

   rtimer_clock_t timestamp;// Time when the message was send

}; 

int initialize_tsch_schedule(void){

 
 
    LOG_PRINT("Initialize tsch schedule\nRemoving all old slotframes\n");
 
    
    int i, j; 
    // APP_SLOTFRAME_SIZE
    struct tsch_slotframe *sf_common = tsch_schedule_add_slotframe(APP_SLOTFRAME_HANDLE, APP_SLOTFRAME_SIZE);
    uint16_t slot_offset;
    uint16_t channel_offset;  
    uint8_t link_options;
    
    slot_offset = 0;
    channel_offset = 0;
    int num_links = 1 ;    
    uint16_t remote_id = 0; 
    linkaddr_t addr;  

      if (node_id != 1) {
        
        for (i = 0 ; i <  num_links ; ++i) { 
        
          remote_id = 1 ; 
         
        
        for(j = 0; j < sizeof(addr); j += 2) {
            addr.u8[j + 1] = remote_id & 0xff;
            addr.u8[j + 0] = remote_id >> 8;
          } 
        }  
          slot_offset = 1 ; //random_rand() % APP_UNICAST_TIMESLOT;
          channel_offset = 2; //random_rand() % APP_CHANNEL_OFSETT ;
          /* Warning: LINK_OPTION_SHARED cannot be configured, as with this schedule
          * backoff windows will not be reset correctly! */
          link_options = LINK_OPTION_TX;

          tsch_schedule_add_link(sf_common,
              link_options,
              LINK_TYPE_NORMAL, &addr,
              slot_offset, channel_offset,0);
          } 
        else { 
          remote_id = 2 ; 
         
        
        for(j = 0; j < sizeof(addr); j += 2) {
            addr.u8[j + 1] = remote_id & 0xff;
            addr.u8[j + 0] = remote_id >> 8;
          } 
          
          slot_offset = 1 ; //random_rand() % APP_UNICAST_TIMESLOT;
          channel_offset = 2; //random_rand() % APP_CHANNEL_OFSETT ;
          /* Warning: LINK_OPTION_SHARED cannot be configured, as with this schedule
          * backoff windows will not be reset correctly! */
          link_options = LINK_OPTION_RX;

          tsch_schedule_add_link(sf_common,
              link_options,
              LINK_TYPE_NORMAL, &addr,
              slot_offset, channel_offset,0);
        }  
          return remote_id;
      
    }
     
      
  

static void 
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  struct latency_structure msg;
  rtimer_clock_t latency;

  packetbuf_copyto( &msg ); // Copy the message from the packet buffer to the structure called msg

#if TIMESYNCH_CONF_ENABLED
  latency = timesynch_time() - msg.timestamp;
#else
  latency = 0;
#endif

  printf("broadcast message received from %d.%d with latency %lu ms\n",
         from->u8[0], from->u8[1], (1000L * latency) / RTIMER_ARCH_SECOND );
} 

static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast; 

PROCESS_THREAD(node_process, ev, data)
{ 
  struct latency_struct *msg ;  
  static struct etimer periodic_timer; 
  
  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();   

  broadcast_open(&broadcast, 129, &broadcast_call);
  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
  /* Initialization; `rx_packet` is the function for packet reception */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, rx_packet);
  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);  
  // para mudar o slotframe;  
   if(node_id == 1) {  /* Running on the root? */
     NETSTACK_ROUTING.root_start();
   } 
  // ativa os protocolos da camda de rede 
  NETSTACK_MAC.on(); 


  /* Main loop */
  while(1) { 
    aux_id = initialize_tsch_schedule();  
    LOG_INFO("%d \n",aux_id);  
    #if TIMESYNCH_CONF_ENABLED
      msg->timestamp = timesynch_time();  
    #else
      msg->timestamp = 0;
    #endif
   
    // energest_flush();

    // printf("\nEnergest:\n");
    // printf(" CPU          %4lus LPM      %4lus DEEP LPM %4lus  Total time %lus\n",
    //        to_seconds(energest_type_time(ENERGEST_TYPE_CPU)),
    //        to_seconds(energest_type_time(ENERGEST_TYPE_LPM)),
    //        to_seconds(energest_type_time(ENERGEST_TYPE_DEEP_LPM)),
    //        to_seconds(ENERGEST_GET_TOTAL_TIME()));
    // printf(" Radio LISTEN %4lus TRANSMIT %4lus OFF      %4lus\n",
    //        to_seconds(energest_type_time(ENERGEST_TYPE_LISTEN)),
    //        to_seconds(energest_type_time(ENERGEST_TYPE_TRANSMIT)),
    //        to_seconds(ENERGEST_GET_TOTAL_TIME()
    //                   - energest_type_time(ENERGEST_TYPE_TRANSMIT)
    //                   - energest_type_time(ENERGEST_TYPE_LISTEN)));
    
     
    //verify_packs();

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
     // Do not send the normal/original 'hello' message. Instead, send a message with the timestamp.
    packetbuf_copyfrom(msg,  sizeof(*msg) ); // This message msg includes the timestamp
    //packetbuf_copyfrom("Hello", 6);
    broadcast_send(&broadcast);
    printf("broadcast message sent\n");
    etimer_set(&periodic_timer, SEND_INTERVAL);
  }

  PROCESS_END(); 
}