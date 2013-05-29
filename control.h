#include "contiki.h"
#include "contiki-net.h"
#include "settings.h"
#include "node-id.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "acc-sensor.h"
#include "gyro-sensor.h"
#include "pressure-sensor.h"
#include <i2c.h>

#include "net/uip-debug.h"
#define DEBUG DEBUG_PRINT
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])


//Buttons
#define CROSS 0
#define CIRCLE 1
#define TRIANGLE 2
#define SQUARE 3
//#define START 4
//#define SELECT 5
//#define BUTTON_6 6
#define MOVE_CONTROL 7
 

//Movement
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define START 4
#define SELECT 5
//#define R1 6
//#define R2 7


//                 fest A  R/W
//                 |--||-|  |
#define IC1_ADDR_R 0b01000001
