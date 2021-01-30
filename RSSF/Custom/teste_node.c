

#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/mac/tsch/tsch.h"
#include "lib/random.h"
#include "sys/node-id.h"  
#include "sys/log.h"
#include "sys/energest.h" 
//#include "simple-energest.h"


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
#define APP_UNICAST_TIMESLOT 26 
#define APP_CHANNEL_OFSETT 10  
//#define APP_MAX_SLOTFRAME_SIZE 21

static void 

initialize_tsch_schedule()
{
  int i, j;  
  // APP_SLOTFRAME_SIZE
  struct tsch_slotframe *sf_common = tsch_schedule_add_slotframe(APP_SLOTFRAME_HANDLE, APP_SLOTFRAME_SIZE);
  uint16_t slot_offset;
  uint16_t channel_offset; 
  
  //int node_number = tsch_num_nos();
  // random the quantity of links  
  slot_offset = 0;
  channel_offset = 0;
  int num_links = 1 ; 
  tsch_schedule_add_link(sf_common,
      LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED,
      LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
      slot_offset, channel_offset,0); 
  if (node_id == 1) {
     
      uint8_t link_options;
      linkaddr_t addr;   
      // node 2 and 3, sending to node 1 
      uint16_t remote_id = 2; 
      for(j = 0; j < sizeof(addr); j += 2) {
        addr.u8[j + 1] = remote_id & 0xff;
        addr.u8[j + 0] = remote_id >> 8;
      } 
      slot_offset = random_rand() % APP_UNICAST_TIMESLOT;
      channel_offset = random_rand() % APP_CHANNEL_OFSETT;
      /* Warning: LINK_OPTION_SHARED cannot be configured, as with this schedule
      * backoff windows will not be reset correctly! */
      link_options = LINK_OPTION_RX;

      tsch_schedule_add_link(sf_common,
          link_options,
          LINK_TYPE_NORMAL, &addr,
          slot_offset, channel_offset,0);
    }
    if (node_id == 2 ){  
      for (i = 0 ; i <  num_links ; ++i) { 

      uint8_t link_options;
      linkaddr_t addr;  
      uint16_t remote_id = 1 ; // sort_node_to_create_link(node_id); 
      for(j = 0; j < sizeof(addr); j += 2) {
        addr.u8[j + 1] = remote_id & 0xff;
        addr.u8[j + 0] = remote_id >> 8;
      } 
      slot_offset = random_rand() % APP_UNICAST_TIMESLOT;
      channel_offset = random_rand() % APP_CHANNEL_OFSETT ;
      /* Warning: LINK_OPTION_SHARED cannot be configured, as with this schedule
      * backoff windows will not be reset correctly! */
      link_options =  LINK_OPTION_TX;

      tsch_schedule_add_link(sf_common,
          link_options,
          LINK_TYPE_NORMAL, &addr,
          slot_offset, channel_offset,0);
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
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_(", seqnum %" PRIu32 "\n", seqnum); 
    
    // pode limpar o arquivo aqui  

  }
} 


PROCESS_THREAD(node_process, ev, data)
{
  static struct simple_udp_connection udp_conn;
  static struct etimer periodic_timer;
  static uint32_t seqnum;
  uip_ipaddr_t dst;
  int counter_pack = 0 ; 
  PROCESS_BEGIN();
  initialize_tsch_schedule();
  /* Initialization; `rx_packet` is the function for packet reception */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, rx_packet);
  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  
  if(node_id == 1) {  /* Running on the root? */
    NETSTACK_ROUTING.root_start();
  }

  /* Main loop */ 
  while(1) { 
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer)); 
    SCHEDULE_static();  
    //simple_energest_step(); 
    if(NETSTACK_ROUTING.node_is_reachable()
       && NETSTACK_ROUTING.get_root_ipaddr(&dst)){
      /* Send network uptime timestamp to the network root node */
      seqnum++;  
      printf("Packages counts : %d",counter_pack);
      LOG_INFO("Send to ");
      LOG_INFO_6ADDR(&dst);
      LOG_INFO_(", seqnum %" PRIu32 "\n", seqnum);
      simple_udp_sendto(&udp_conn, &seqnum, sizeof(seqnum), &dst); 
      counter_pack++ ;
    }
    etimer_set(&periodic_timer, SEND_INTERVAL);
  }

  PROCESS_END();
}