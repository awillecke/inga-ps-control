#include "control.h"

PROCESS(default_app_process, "Controller process");
AUTOSTART_PROCESSES(&default_app_process);

static void broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from) {
  //
}

static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
static struct etimer timer;

static int16_t x = 0;
static int count = 0;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(default_app_process, ev, data) {

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  broadcast_open(&broadcast, 129, &broadcast_call);

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(acc_sensor);
  SENSORS_ACTIVATE(gyro_sensor);
   	
  etimer_set(&timer, CLOCK_SECOND * 0.05);

  while (1) { 
    PROCESS_YIELD();
    if ( etimer_expired(&timer) ) {
      etimer_set(&timer, CLOCK_SECOND*0.5);
      #ifdef CONTROL == 0 
      packetbuf_copyfrom(CROSS, 2);
      broadcast_send(&broadcast);
      printf("%d\n", CONTROL);  
      #endif
      #ifdef CONTROL == 1
      packetbuf_copyfrom(CIRCLE, 2);
      broadcast_send(&broadcast);
      printf("%d\n", CONTROL); 
      #endif
      #ifdef CONTROL == 2
      packetbuf_copyfrom(TRIANGLE, 2);
      broadcast_send(&broadcast);
      printf("%d\n", CONTROL); 
      #endif
      #ifdef CONTROL == 3
      packetbuf_copyfrom(SQUARE, 2);
      broadcast_send(&broadcast);
      printf("%d\n", CONTROL); 
      #endif
 
    }
  PROCESS_END();
  }
}
/*---------------------------------------------------------------------------*/
