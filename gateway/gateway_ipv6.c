#include "gateway.h"
/*---------------------------------------------------------------------------*/
PROCESS(gateway, "gateway main processs");
AUTOSTART_PROCESSES(&gateway);
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
     
    PORTA = 0b00000011;
    DDRA = 0b00000011;
  
    PROCESS_BEGIN();


    print_local_addresses();

    server_conn = udp_new(NULL, UIP_HTONS(3003), NULL);
    udp_bind(server_conn, UIP_HTONS(3002));

    while(1) {
        PROCESS_YIELD();
        //printf("back\n");
        if(ev == tcpip_event) {
            //printf("tcp event\n");
            udp_receive_data();

            if (recv == 2) {  //TRI
                PORTA &= ~(1<<PA0);
                //PORTA |= (1<<PA0);
                printf("SETTING IO\n");
                etimer_set(&timer, CLOCK_SECOND*0.01);//0.05);
                PROCESS_YIELD();
                printf("UNSET IO\n");
                PORTA |= (1<<PA0);
                //PORTA &= ~(1<<PA0);
                recv = 0;
            } 
            else if (recv == 3) {  //SQUARE
                PORTA &= ~(1<<PA1);   
                printf("SETTING IO\n");
                etimer_set(&timer, CLOCK_SECOND*0.01);//0.05);
                PROCESS_YIELD();
                printf("UNSET IO\n");
                PORTA |= (1<<PA1);  
                recv = 0;
            }      
         }  
    }  
    PROCESS_END();
}
/*--------------------------------------------------------------------------------------*/
int set_button(uint16_t output)
{
    uint8_t ret,            
            ic1 = (uint8_t)output,
            ic2 = (uint8_t)(output >> 8);
    
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
/*--------------------------------------------------------------------------------------*/
