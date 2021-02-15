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
#include "net/mac/tsch/tsch.h"
#include "lib/random.h"
#include "sys/node-id.h"  
#include "net/nullnet/nullnet.h"  

#include "sys/log.h"
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
//#define APP_SLOTFRAME_HANDLE 1
/* Put all unicast cells on the same timeslot (for demonstration purposes only) */
//#define APP_UNICAST_TIMESLOT 1

// static void
// initialize_tsch_schedule(void)
// {
//   int i, j;
//   struct tsch_slotframe *sf_common = tsch_schedule_add_slotframe(APP_SLOTFRAME_HANDLE, APP_SLOTFRAME_SIZE);
//   uint16_t slot_offset;
//   uint16_t channel_offset;

//   /* A "catch-all" cell at (0, 0) */
//   slot_offset = 0;
//   channel_offset = 0;
//   tsch_schedule_add_link(sf_common,
//       LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED,
//       LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
//       slot_offset, channel_offset, 1);

//   for (i = 0; i < TSCH_SCHEDULE_MAX_LINKS - 1; ++i) {
//     uint8_t link_options;
//     linkaddr_t addr;
//     uint16_t remote_id = i + 1;

//     for(j = 0; j < sizeof(addr); j += 2) {
//       addr.u8[j + 1] = remote_id & 0xff;
//       addr.u8[j + 0] = remote_id >> 8;
//     }

//     /* Add a unicast cell for each potential neighbor (in Cooja) */
//     /* Use the same slot offset; the right link will be dynamically selected at runtime based on queue sizes */
//     slot_offset = APP_UNICAST_TIMESLOT;
//     channel_offset = i;
//     /* Warning: LINK_OPTION_SHARED cannot be configured, as with this schedule
//      * backoff windows will not be reset correctly! */
//     link_options = remote_id == node_id ? LINK_OPTION_RX : LINK_OPTION_TX;

//     tsch_schedule_add_link(sf_common,
//         link_options,
//         LINK_TYPE_NORMAL, &addr,
//         slot_offset, channel_offset, 1);
//   }
// } 

void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(unsigned)){
    unsigned count;
    memcpy(&count, data, sizeof(count));
    LOG_INFO("Received %u from ", count);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
  }
}  
PROCESS_THREAD(node_process, ev, data)
{
  static struct etimer periodic_timer;
 

  PROCESS_BEGIN();


//tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
/* MAC_CONF_WITH_TSCH */
  static linkaddr_t dest_addr;  
  int remote_id = initialize_tsch_schedule();  
  for(int j = 0; j < sizeof(dest_addr); j += 2) {
        dest_addr.u8[j + 1] = remote_id & 0xff;
        dest_addr.u8[j + 0] = remote_id >> 8;
    }   
  
  /* Initialize NullNet */
  uint8_t payload[64] = { 0 };
  nullnet_buf = payload; /* Point NullNet buffer to 'payload' */
  nullnet_len = 2; /* Tell NullNet that the payload length is two bytes */
  nullnet_set_input_callback(input_callback);

  if(!linkaddr_cmp(&dest_addr, &linkaddr_node_addr)) {
    etimer_set(&periodic_timer, SEND_INTERVAL);
    while(1) { 

      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
      LOG_INFO("Sending %u to ", count);
      LOG_INFO_LLADDR(&dest_addr);
      LOG_INFO_("\n");

      NETSTACK_NETWORK.output(&dest_addr);
      count++;
      etimer_reset(&periodic_timer);
    }
  }

  PROCESS_END();
}