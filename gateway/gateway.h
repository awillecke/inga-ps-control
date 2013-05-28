#include "contiki.h"
#include <util/delay.h>
#include <stdlib.h>
#include "acc-sensor.h"
#include "gyro-sensor.h"
#include "pressure-sensor.h"
#include <stdio.h>
#include "net/rime.h"
#include "contiki-net.h"
#include "settings.h"
#include "node-id.h"
#include <string.h>
#include "avr/portpins.h"
#include "net/uip-debug.h"

#define DEBUG_PRINT 1
#define DEBUG DEBUG_PRINT
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

//Buttons
#define CROSS 0
#define CIRCLE 1
#define TRIANGLE 2
#define SQUARE 3
#define UP 4
#define DOWN 5
#define LEFT 6
#define RIGHT 7
#define L1 8
#define L2 9
#define R1 10
#define R2 11
#define START 12
#define SELECT 13


#define CONTROL 13

//                 fest A  R/W
//                 |--||-|  |
#define IC1_ADDR_W 0b01000000
#define IC2_ADDR_W 0b01001110

int set_button(uint16_t output);
