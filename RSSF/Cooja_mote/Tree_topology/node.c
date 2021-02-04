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
#include "sys/log.h"
#include "sys/energest.h"  


#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
#define UDP_PORT	8765
#define SEND_INTERVAL		  (60 * CLOCK_SECOND) 
PROCESS(node_process, "TSCH Schedule Node");
AUTOSTART_PROCESSES(&node_process);

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
/* Put all unicast cells on the same timeslot (for demonstration purposes only) */
#define APP_UNICAST_TIMESLOT 16 
#define APP_CHANNEL_OFSETT 16


static unsigned long
to_seconds(uint64_t time)
{
  return (unsigned long)(time / ENERGEST_SECOND);
}

static void 
initialize_tsch_schedule()
{ 
  int i, j;  
  // APP_SLOTFRAME_SIZE
  struct tsch_slotframe *sf_common = tsch_schedule_add_slotframe(APP_SLOTFRAME_HANDLE, APP_SLOTFRAME_SIZE);
  uint16_t slot_offset;
  uint16_t channel_offset; 
  
  slot_offset = 0;
  channel_offset = 0;
  int num_links = 1 ; 
  tsch_schedule_add_link(sf_common,
      LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED,
      LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
      slot_offset, channel_offset,0); 
  
  
  if (node_id != 1) {
    if (node_id == 2 || node_id == 3){ 
      uint8_t link_options;
      linkaddr_t addr;   
      uint16_t remote_id = 1; 
      for(j = 0; j < sizeof(addr); j += 2) {
        addr.u8[j + 1] = remote_id & 0xff;
        addr.u8[j + 0] = remote_id >> 8;
      } 
      slot_offset = random_rand() % APP_UNICAST_TIMESLOT;
      channel_offset = random_rand() % APP_CHANNEL_OFSETT;
      /* Warning: LINK_OPTION_SHARED cannot be configured, as with this schedule
      * backoff windows will not be reset correctly! */
      link_options = remote_id == node_id ? LINK_OPTION_RX : LINK_OPTION_TX;

      tsch_schedule_add_link(sf_common,
          link_options,
          LINK_TYPE_NORMAL, &addr,
          slot_offset, channel_offset,0);
    }
    else{  
      for (i = 0 ; i <  num_links ; ++i) { 

      uint8_t link_options;
      linkaddr_t addr;  
      uint16_t remote_id = sort_node_to_create_link(node_id); 
      for(j = 0; j < sizeof(addr); j += 2) {
        addr.u8[j + 1] = remote_id & 0xff;
        addr.u8[j + 0] = remote_id >> 8;
      }  
      slot_offset = random_rand() % APP_UNICAST_TIMESLOT;
      channel_offset = random_rand() % APP_CHANNEL_OFSETT ;
      /* Warning: LINK_OPTION_SHARED cannot be configured, as with this schedule
      * backoff windows will not be reset correctly! */
      link_options = remote_id == node_id ? LINK_OPTION_RX : LINK_OPTION_TX;

      tsch_schedule_add_link(sf_common,
          link_options,
          LINK_TYPE_NORMAL, &addr,
          slot_offset, channel_offset,0);
      }

    } 
  }
  
}

static void
rx_packet(struct simple_udp_connection *c,
          const uip_ipaddr_t *sender_addr,
          uint16_t sender_port,
          const uip_ipaddr_t *receiver_addr,
          uint16_t receiver_port,
          const uint8_t *data,
          uint16_t datalen)
{
  uint32_t seqnum;

  if(datalen >= sizeof(seqnum)) {
    memcpy(&seqnum, data, sizeof(seqnum));

    LOG_INFO("Received from ");
    //LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_(", seqnum %" PRIu32 "\n", seqnum); 
   

  }
} 


PROCESS_THREAD(node_process, ev, data)
{
  //static struct simple_udp_connection udp_conn;
  static struct etimer periodic_timer;
  //static uint32_t seqnum;
  //uip_ipaddr_t dst;   
  PROCESS_BEGIN();
  initialize_tsch_schedule(); 
  /* Initialization; `rx_packet` is the function for packet reception */
  //simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, rx_packet);
  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  
       
  /* Main loop */ 
  while(1) { 
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));  

   // SCHEDULE_static();    
    
    energest_flush();

    printf("\nEnergest:\n");
    printf(" CPU          %4lus LPM      %4lus DEEP LPM %4lus  Total time %lus\n",
           to_seconds(energest_type_time(ENERGEST_TYPE_CPU)),
           to_seconds(energest_type_time(ENERGEST_TYPE_LPM)),
           to_seconds(energest_type_time(ENERGEST_TYPE_DEEP_LPM)),
           to_seconds(ENERGEST_GET_TOTAL_TIME()));
    printf(" Radio LISTEN %4lus TRANSMIT %4lus OFF      %4lus\n",
           to_seconds(energest_type_time(ENERGEST_TYPE_LISTEN)),
           to_seconds(energest_type_time(ENERGEST_TYPE_TRANSMIT)),
           to_seconds(ENERGEST_GET_TOTAL_TIME()
                      - energest_type_time(ENERGEST_TYPE_TRANSMIT)
                      - energest_type_time(ENERGEST_TYPE_LISTEN)));
    
    
    
   
      /* Send network uptime timestamp to the network root node */
      // seqnum++;  
      // LOG_INFO("Send to ");
      // LOG_INFO_6ADDR(&dst);
      // LOG_INFO_(", seqnum %" PRIu32 "\n", seqnum);
      // simple_udp_sendto(&udp_conn, &seqnum, sizeof(seqnum), &dst); 
    
    }
    etimer_set(&periodic_timer, SEND_INTERVAL);
   }

  PROCESS_END();
}