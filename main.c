#include "contiki.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "acc-sensor.h"
#include "gyro-sensor.h"
#include "pressure-sensor.h"


#define DEBUG                                   0

#include <stdio.h> /* For printf() */

PROCESS(default_app_process, "Hello world process");
AUTOSTART_PROCESSES(&default_app_process);
 
static struct etimer timer;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(default_app_process, ev, data) {
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(acc_sensor);
  SENSORS_ACTIVATE(gyro_sensor);
   	
  etimer_set(&timer, CLOCK_SECOND * 0.05);

  while (1) { 
    PROCESS_YIELD();
    if ( etimer_expired(&timer) ) {
       etimer_set(&timer, CLOCK_SECOND*0.5);

       printf("Lage: X: %d, Y: %d, Z: %d. ",gyro_sensor.value(GYRO_X), gyro_sensor.value(GYRO_Y), gyro_sensor.value(GYRO_Z));
       printf("Beschl.: X: %d, Y: %d, Z: %d.\n",acc_sensor.value(ACC_X_RAW), acc_sensor.value(ACC_Y_RAW), acc_sensor.value(ACC_Z_RAW));

       }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
