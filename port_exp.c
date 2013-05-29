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


//                 fest A  R/W
//                 |--||-|  |
#define IC1_ADDR_W 0b01000000
#define IC2_ADDR_W 0b01001110

int set_button(uint16_t output)
{
    uint8_t ret;
            
    uint8_t ic1 = (uint8_t)output;
    uint8_t ic2 = (uint8_t)(output >> 8);
    
    printf("ic1: %d, ic2: %d\n", ic1, ic2);
   
    i2c_init();

    if(i2c_start(IC1_ADDR_W))
    {
        ret = i2c_write(output);
    }
    else
    {
        printf("i2c_start failed: IC1\n");
    }

    i2c_stop();
    
    if(i2c_start(IC2_ADDR_W))
    {
        ret = i2c_write(output);
    }
    else
    {
        printf("i2c_start failed: IC2\n");
    }    
  
  return ret;     
}
