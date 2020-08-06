
#include "net/mac/tsch/tsch.h" 
#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "lib/random.h"
#include "sys/node-id.h"
#include "sys/log.h"

#include <stdio.h>s
#include <stdlib.h>
#include "rgraph.h"
#include "conf.h"
#include "criaDOT.h"
#define temp_canais 23
#define peso 1 

void executa(int **aloca_canal, int tempo, int **mapa_graf_conf, int *pacote_entregue, int raiz, int *pacotes);
int *alocaPacotes(int num_no); 
char *colect_addres(char *ex); 
static void initialize_tsch_schedule_root();

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
  linkaddr_t addr; 
  linkaddr_t addr_r ; 
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
for(int i = 1 ; i <= 3; i ++){  
        
        if(i == 1){    
            // no 1 envia para o no 2 
            for(j = 0; j < sizeof(addr); j += 2) {
            addr.u8[j + 1] = 2 & 0xff;
            addr.u8[j + 0] = 2 >> 8;
            }
            link_options =  LINK_OPTION_TX; 
            tsch_schedule_add_link(sf_min, link_options, LINK_TYPE_NORMAL, &addr ,0 ,1);
        }
        if(i == 2 ){   
            // no 2 recebe do no 1
            for(j = 0; j < sizeof(addr); j += 2) {
            addr.u8[j + 1] = 1 & 0xff;
            addr.u8[j + 0] = 1 >> 8;
            }  
            link_options =  LINK_OPTION_RX; 
            tsch_schedule_add_link(sf_min, link_options, LINK_TYPE_NORMAL, &addr ,0 ,1); 
            // no 2 envia para o no 3 
            for(j = 0; j < sizeof(addr); j += 2) {
            addr.u8[j + 1] = 3 & 0xff;
            addr.u8[j + 0] = 3 >> 8;
            }
            link_options =  LINK_OPTION_TX;  
            tsch_schedule_add_link(sf_min, link_options, LINK_TYPE_NORMAL, &addr,0,2);
        else {  
            // no 3 recebe do no 2 
            for(j = 0; j < sizeof(addr); j += 2) {
            addr.u8[j + 1] = 2 & 0xff;
            addr.u8[j + 0] = 2 >> 8;
                }  
            link_options =  LINK_OPTION_RX; 
            tsch_schedule_add_link(sf_min, link_options, LINK_TYPE_NORMAL, &addr ,0 ,2); 
            
        }
                             
}

/* 
 caso precise incluir no makefile
PROJECT_SOURCEFILES += rsa_get_size.c 
MODULES += ./tomcrypt

*/

 
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
  NETSTACK_MAC.on();
  if(node_id == 1) {  /* Running on the root? */
    NETSTACK_ROUTING.root_start(); 
    
    

  } 
  initialize_tsch_schedule_global(); 

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


