#include "contiki.h"
#include <util/delay.h>
#include <stdlib.h>
#include "acc-sensor.h"
#include "gyro-sensor.h"
#include "pressure-sensor.h"
#include <stdio.h>
#include "contiki-net.h"
#include "settings.h"
#include "node-id.h"
#include <string.h>
#include "avr/portpins.h"
#include "net/uip-debug.h"
#include <i2c.h>
#include "sys/ctimer.h"

#define DEBUG_PRINT 1
#define DEBUG DEBUG_PRINT
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

//Buttons
#define CROSS 0
#define CIRCLE 1
#define TRIANGLE 2
#define SQUARE 3
#define START 4
#define SELECT 5
#define BUTTON_6 6
#define MOVE_CONTROL 7
 

//Movement
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define L1 4
#define L2 5
#define R1 6
#define R2 7


#define BUTTON_HOLD 0.05

#define CONTROL 13

//                 fest A  R/W
//                 |--||-|  |
#define IC1_ADDR_W 0b01000000
#define IC2_ADDR_W 0b01001110

static void set_control(void *output_ptr);
static void unset_control(void *output_ptr);

static uint8_t log_2(uint8_t n);
