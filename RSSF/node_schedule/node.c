
#include "Schedule.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_PORT	8765
#define SEND_INTERVAL		  (60 * CLOCK_SECOND) 



PROCESS(node_process, "TSCH Schedule Node"); 
AUTOSTART_PROCESSES(&node_process);  


#define APP_SLOTFRAME_HANDLE 1
#define APP_UNICAST_TIMESLOT 1
 
 
// cria um  unico slotframe slotframe   
static  void 
initialize_tsch_schedule_global(void){ 
  
  struct tsch_slotframe *sf_common = tsch_schedule_add_slotframe(APP_SLOTFRAME_HANDLE, APP_SLOTFRAME_SIZE);
  uint16_t slot_offset;
  uint16_t channel_offset;

  /* A "catch-all" cell at (0, 0) */
  slot_offset = 0;
  channel_offset = 0;
  tsch_schedule_add_link(sf_common,
      LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED,
      LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
      slot_offset, channel_offset);
 
    
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
        }
} 

PROCESS_THREAD(node_process, ev, data)
{
  static struct simple_udp_connection udp_conn;
  static struct etimer periodic_timer;
  static uint32_t seqnum;
  uip_ipaddr_t dst;

  PROCESS_BEGIN();

  initialize_tsch_schedule_global();

  /* Initialization; `rx_packet` is the function for packet reception */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, rx_packet);
  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  // colocar no ulitmo como nó raiz  
  if(node_id == 1) {  /* Running on the root? */
    NETSTACK_ROUTING.root_start(); 
    initialize_tsch_schedule_root(); 

  }

  /* Main loop */
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    if(NETSTACK_ROUTING.node_is_reachable()
       && NETSTACK_ROUTING.get_root_ipaddr(&dst)) {
      /* Send network uptime timestamp to the network root node */
      seqnum++;
      LOG_INFO("Send to ");
      LOG_INFO_6ADDR(&dst);
      LOG_INFO_(", seqnum %" PRIu32 "\n", seqnum);
      simple_udp_sendto(&udp_conn, &seqnum, sizeof(seqnum), &dst);
    }
    etimer_set(&periodic_timer, SEND_INTERVAL);
  }

  PROCESS_END();
}


