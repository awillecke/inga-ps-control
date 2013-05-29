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
        set_control(recv); 
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
    
    set_control(0);
    set_control(0);
    
    
    print_local_addresses();

    server_conn = udp_new(NULL, UIP_HTONS(3003), NULL);
    udp_bind(server_conn, UIP_HTONS(3002));

    while(1) {
        PROCESS_YIELD();
        if(ev == tcpip_event) {
            udp_receive_data();
         }  
    }  
    PROCESS_END();
}
/*--------------------------------------------------------------------------------------*/
int set_control(uint8_t output) {

    uint8_t ret = -1;

    if (((output & 0b10000000) >> MOVE_CONTROL) == 0) {
        if(i2c_start(IC1_ADDR_W) == 0) {
            ret = i2c_write(output);
            i2c_stop();
        } 
        else {
            printf("i2c_start failed: IC1\n");
        }
    }
    else {
        if(i2c_start(IC2_ADDR_W) == 0) {
            ret = i2c_write(output);
            i2c_stop();
        } 
        else {
            printf("i2c_start failed: IC2\n");
        }
    }
    
    return ret;     
}
/*--------------------------------------------------------------------------------------*/
