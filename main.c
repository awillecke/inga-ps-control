#include "contiki.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "acc-sensor.h"
#include "gyro-sensor.h"
#include "pressure-sensor.h"

// Hallo Alex
#define DEBUG                                   0

#include <stdio.h> /* For printf() */

PROCESS(default_app_process, "Hello world process");
AUTOSTART_PROCESSES(&default_app_process);
 
static struct etimer timer;
static int16_t x = 0;
static int count = 0;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(default_app_process, ev, data) {

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(acc_sensor);
  SENSORS_ACTIVATE(gyro_sensor);
   	
  etimer_set(&timer, CLOCK_SECOND * 0.05);

  while (1) { 
    PROCESS_YIELD();
    if ( etimer_expired(&timer) ) {
       etimer_set(&timer, CLOCK_SECOND*0.05);

//       printf("Lage: X: %4d, Y: %4d, Z: %4d. ",gyro_sensor.value(GYRO_X_RAW), gyro_sensor.value(GYRO_Y_RAW), gyro_sensor.value(GYRO_Z_RAW));
//       printf("Beschl.: X: %6d, Y: %6d, Z: %6d.\n",acc_sensor.value(ACC_X_RAW), acc_sensor.value(ACC_Y_RAW), acc_sensor.value(ACC_Z_RAW));
         x = acc_sensor.value(ACC_X_RAW);
         if (x > 400) {
           printf("BOX!!! nr %d over 400\n", count);
           count++;
         }
       }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
