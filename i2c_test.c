#include <stdio.h>
#include <i2c.h>
#include "contiki.h"
#ifdef INGA_REVISION
#include <avr/eeprom.h>
#include "settings.h"
#include <util/delay.h>
#endif
#include <stdlib.h>
#include "node-id.h"
#include "leds.h"

//                      A R/W
//                 |--||-||
#define IC1_ADDR_W 0b01000000
#define IC2_ADDR_W 0b01001110

#define DEBUG               0

/*---------------------------------------------------------------------------*/
PROCESS(default_app_process, "Hello world process");
AUTOSTART_PROCESSES(&default_app_process);
/*---------------------------------------------------------------------------*/
static struct etimer timer;
PROCESS_THREAD(default_app_process, ev, data)
{
  PROCESS_BEGIN();

  uint8_t output = 0b11110000;
  uint8_t ret;
   
  i2c_init();

  ret = i2c_start(IC1_ADDR_W);
  printf("start: %d\n", ret);

  ret = i2c_write(output);
  printf("write: %d\n", ret);

  i2c_stop();

  ret = i2c_start(IC2_ADDR_W);
  printf("start: %d\n", ret);

  ret = i2c_write(output);
  printf("write: %d\n", ret);
      
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
