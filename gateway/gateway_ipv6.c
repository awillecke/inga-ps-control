#include "gateway.h"
/*---------------------------------------------------------------------------*/
PROCESS(gateway, "gateway main processs");
AUTOSTART_PROCESSES(&gateway);

static uint16_t port = 0b1111111111111111;
/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *server_conn;
static int recv;
static struct etimer timer;

static void udp_receive_data(void) {
    if(uip_newdata()) {
        recv = atoi((char *)uip_appdata);
        printf("Empfange Packet: %d\n", recv);       
    }
}

static void print_local_addresses(void) {
    int i;
    uint8_t state;
    PRINTF("Server IPv6 addresses: ");
    for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
        state = uip_ds6_if.addr_list[i].state;
        if(uip_ds6_if.addr_list[i].isused && (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
          PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
          PRINTF("\n");
        }
    }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gateway, ev, data) {
  
    PROCESS_BEGIN();

    i2c_init();
    
    set_button(0);
    set_button(0);
    
    
    print_local_addresses();

    server_conn = udp_new(NULL, UIP_HTONS(3003), NULL);
    udp_bind(server_conn, UIP_HTONS(3002));

    while(1) {
        PROCESS_YIELD();
        if(ev == tcpip_event) {
            udp_receive_data();
            if (recv == CROSS) {  //CROSS
                set_button(CROSS);
                etimer_set(&timer, CLOCK_SECOND*BUTTON_HOLD);
                PROCESS_YIELD();
                set_button(CROSS);
                recv = 0;
            }
            else if (recv == CIRCLE) {  //CIRCLE
                set_button(CIRCLE);
                etimer_set(&timer, CLOCK_SECOND*BUTTON_HOLD);
                PROCESS_YIELD();
                set_button(CIRCLE);
                recv = 0;
            }
            else if (recv == TRIANGLE) {  //TRI
                set_button(TRIANGLE);
                etimer_set(&timer, CLOCK_SECOND*BUTTON_HOLD);
                PROCESS_YIELD();
                set_button(TRIANGLE);
                recv = 0;
            } 
            else if (recv == SQUARE) {  //SQUARE
                set_button(SQUARE);
                etimer_set(&timer, CLOCK_SECOND*BUTTON_HOLD);
                PROCESS_YIELD();
                set_button(SQUARE);
                recv = 0;
            }
            else if (recv == UP) {  //UP
                set_button(UP);
                etimer_set(&timer, CLOCK_SECOND*BUTTON_HOLD);
                PROCESS_YIELD();
                set_button(UP);
                recv = 0;
            }
            else if (recv == DOWN) {  //DOWN
                set_button(DOWN);
                etimer_set(&timer, CLOCK_SECOND*BUTTON_HOLD);
                PROCESS_YIELD();
                set_button(DOWN);
                recv = 0;
            }
            else if (recv == LEFT) {  //LEFT
                set_button(LEFT);
                etimer_set(&timer, CLOCK_SECOND*BUTTON_HOLD);
                PROCESS_YIELD();
                set_button(LEFT);
                recv = 0;
            }
            else if (recv == RIGHT) {  //RIGHT
                set_button(RIGHT);
                etimer_set(&timer, CLOCK_SECOND*BUTTON_HOLD);
                PROCESS_YIELD();
                set_button(RIGHT);
                recv = 0;
            }      
         }  
    }  
    PROCESS_END();
}
/*--------------------------------------------------------------------------------------*/
int set_button(uint8_t output)
{
    uint16_t tmp = 0;
    
    tmp = (1 << output);
  //  printf("tmp:  %d\n", tmp);
    port ^= tmp;
//    printf("port: %d\n", port);
    
    uint8_t ret = -1,            
            ic1 = (uint8_t)port,
            ic2 = (uint8_t)(port >> 8);   
            
    printf("ic1: %d, ic2: %d\n", ic1, ic2);
   
    //i2c_init();

    if(i2c_start(IC1_ADDR_W) == 0)
    {
        ret = i2c_write(ic1);
    }
    else
    {
        printf("i2c_start failed: IC1\n");
    }

    i2c_stop();

    if(i2c_start(IC2_ADDR_W) == 0)
    {
        ret = i2c_write(ic2);
    }
    else
    {
        printf("i2c_start failed: IC2\n");
    }    
    
    i2c_stop();
    
    return ret;     
}
/*--------------------------------------------------------------------------------------*/
