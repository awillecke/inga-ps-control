#include "contiki.h"
#include "net/rime.h"
#include "random.h"
#include <string.h>

#include "dev/button-sensor.h"
#include "avr/portpins.h"

#include "dev/leds.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
static struct etimer et;
static struct etimer timer;
static struct etimer recvtimer;
static int recv = 0;
static void broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from) {
   printf("broadcast message received from %d.%d: '%s'\n", from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
   recv = atoi((char *)packetbuf_dataptr());
   //printf("%d\n",recv);
   etimer_set(&recvtimer, CLOCK_SECOND*0.01);
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data) {
    
    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
    
    PORTA = 0b00000011;
    DDRA = 0b00000011;
  
    PROCESS_BEGIN();

    broadcast_open(&broadcast, 22, &broadcast_call);
    etimer_set(&et, CLOCK_SECOND*2);//0.05);
                
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        //printf("end yield\n");
        //if(!(ev == timer)) {
            if (recv == 2) {  //TRI
                PORTA &= ~(1<<PA0);
                printf("SETTING IO\n");
                etimer_set(&timer, CLOCK_SECOND*0.02);//0.05);
                PROCESS_YIELD();
                printf("UNSET IO\n");
                PORTA |= (1<<PA0);
                recv = 0;
            } 
            else if (recv == 3) {  //SQUARE
                PORTA &= ~(1<<PA1);   
                printf("SETTING IO\n");
                etimer_set(&timer, CLOCK_SECOND*0.02);//0.05);
                PROCESS_YIELD();
                printf("UNSET IO\n");
                PORTA |= (1<<PA1);  
                recv = 0;
            }      
         //}
         etimer_set(&et, CLOCK_SECOND*0.01);//0.05);
                
    }  
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/*
 */
