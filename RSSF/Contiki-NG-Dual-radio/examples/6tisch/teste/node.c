#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/radio.h"

PROCESS(process_dual_radio, "Dual Radio");
AUTOSTART_PROCESSES(&process_dual_radio);

// Callback function for receiving data
static void recv_data_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest)
{
  printf("Received data: ");
  for (int i = 0; i < len; i++)
  {
    printf("%c", ((uint8_t *)data)[i]);
  }
  printf(" from ");
  printf("%d.%d\n", src->u8[0], src->u8[1]);
}

PROCESS_THREAD(example_process, ev, data)
{
  PROCESS_BEGIN();

  // Initialize NullNet
  nullnet_set_input_callback(recv_data_callback);
  nullnet_buf = (uint8_t *)&recv_data_callback;

  // Set radio channel and power for Radio 1
  NETSTACK_RADIO.init();
  NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, 25);
  NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, 0);

  // Set radio channel and power for Radio 2
  NETSTACK_RADIO.init();
  NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, 26);
  NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, 0);

  // Start Radio 1
  NETSTACK_RADIO.on();

  // Start Radio 2
  radio_on(1);

  while (1)
  {
    // Send data with Radio 1
    static uint8_t data1[] = "Hello from Radio 1!";
    NETSTACK_RADIO.send(data1, sizeof(data1));

    // Send data with Radio 2
    static uint8_t data2[] = "Hello from Radio 2!";
    radio_send(1, data2, sizeof(data2));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);
  }

  PROCESS_END();
}
