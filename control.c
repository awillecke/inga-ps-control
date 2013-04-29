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
  //SENSORS_ACTIVATE(gyro_sensor);
   	
  etimer_set(&timer, CLOCK_SECOND * 0.05);

  while (1) { 
    PROCESS_YIELD();
    if ( etimer_expired(&timer) ) {
      etimer_set(&timer, CLOCK_SECOND*0.05);
      x = acc_sensor.value(ACC_Y_RAW);
      if (x > 380) {
        #if CONTROL == 0 
        packetbuf_copyfrom("0", 2);
        broadcast_send(&broadcast);
        printf("%d: CROSS Nr.%d\n", CONTROL, count);  
        #elif CONTROL == 1
        packetbuf_copyfrom("1", 2);
        broadcast_send(&broadcast);
        printf("%d: CIRCLE Nr.%d\n", CONTROL, count);
        #elif CONTROL == 2
        packetbuf_copyfrom("2", 2);
        broadcast_send(&broadcast);
        printf("%d: TRIANGLE Nr.%d\n", CONTROL, count);
        #elif CONTROL == 3
        packetbuf_copyfrom("3", 2);
        broadcast_send(&broadcast);
        printf("%d: SQUARE Nr.%d\n", CONTROL, count); 
        #endif
        etimer_set(&timer, CLOCK_SECOND*0.3);
        count++;
      } 
    }
  PROCESS_END();
  }
}
/*---------------------------------------------------------------------------*/
