#include "contiki.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/ipv6/simple-udp.h" 


#define UDP_PORT1 12345
#define UDP_PORT2 54321

PROCESS(schedule_example, "Schedule Example");
AUTOSTART_PROCESSES(&schedule_example); 

#define SEND_INTERVAL (CLOCK_SECOND * 5) // Send a packet every 5 seconds

static struct simple_udp_connection udp_connection1;
static struct simple_udp_connection udp_connection2;

PROCESS_THREAD(schedule_example, ev, data)
{
  static struct etimer send_timer1;
  static struct etimer send_timer2;

  PROCESS_BEGIN();

  // Set up the UDP connections
  simple_udp_register(&udp_connection1, UDP_PORT1, NULL, UDP_PORT1, NULL);
  simple_udp_register(&udp_connection2, UDP_PORT2, NULL, UDP_PORT2, NULL);

  while (1)
  {
    // Wait for the send timers to expire
    etimer_set(&send_timer1, SEND_INTERVAL);
    etimer_set(&send_timer2, SEND_INTERVAL);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer1) || etimer_expired(&send_timer2));

    if (etimer_expired(&send_timer1))
    {
      // Create a packet buffer and set the payload for packet 1
      packetbuf_clear();
      packetbuf_set_datalen(sizeof("Packet 1"));
      memcpy(packetbuf_dataptr(), "Packet 1", sizeof("Packet 1"));

      // Send packet 1 using channel 11
      NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, 11); // Set channel to 11
      simple_udp_sendto(&udp_connection1, "dest-ip-address1", UDP_PORT1, packetbuf_dataptr(), packetbuf_datalen());
    }

    if (etimer_expired(&send_timer2))
    {
      // Create a packet buffer and set the payload for packet 2
      packetbuf_clear();
      packetbuf_set_datalen(sizeof("Packet 2"));
      memcpy(packetbuf_dataptr(), "Packet 2", sizeof("Packet 2"));

      // Send packet 2 using channel 26
      NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, 26); // Set channel to 26
      simple_udp_sendto(&udp_connection2, "dest-ip-address2", UDP_PORT2, packetbuf_dataptr(), packetbuf_datalen());
    }
  }

  PROCESS_END();
}
