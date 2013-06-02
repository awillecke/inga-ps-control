#include "gateway.h"
/*---------------------------------------------------------------------------*/
PROCESS(gateway, "gateway main processs");
PROCESS(button_hold, "unsets buttons");
AUTOSTART_PROCESSES(&gateway, &button_hold);
/*---------------------------------------------------------------------------*/
static uint8_t bit_mask_buttons = 0b11111111;
static uint8_t bit_mask_movement = 0b11111111;

static uint8_t recv = 0;
static uint8_t *callback_recv = &recv;

static struct uip_udp_conn *server_conn;
static struct ctimer button_timer[4];
static void (*unset_control) = set_control;
/*--------------------------------------------------------------------------------------*/
static void udp_receive_data(void) {
    if(uip_newdata()) {
        recv = (uint8_t)atoi((char *)uip_appdata);
        printf("Empfange Packet: %d\n", recv);
    }
}
/*--------------------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------------------*/
static int set_control(uint8_t output) {

    uint8_t ret = -1;
    if (((output & 0b10000000) >> MOVE_CONTROL) == 0) {
        bit_mask_buttons ^= output;
        
        if(i2c_start(IC1_ADDR_W) == 0) {
            ret = i2c_write(bit_mask_buttons);
            printf("write: %d\n", output);  
            i2c_stop();
        } 
        else {
            printf("i2c_start failed: IC1\n");
        }
    }
    else {
        bit_mask_movement ^= output;
        
        if(i2c_start(IC2_ADDR_W) == 0) {
            ret = i2c_write(bit_mask_movement);
            printf("write: %d\n", output);
            i2c_stop();
        } 
        else {
            printf("i2c_start failed: IC2\n");
        }
    }
    
    return ret;     
}
/*--------------------------------------------------------------------------------------*/
static uint8_t log2(uint8_t n) {
    /*if (n == 0 && n <= 255)
        return -1;
    uint8_t value = -1;
    while (n) {
        value++;
        n >>= 1;
    }*/
    uint8_t value = 0;
    if (n == 1) {value = 0; }
    if (n == 2) {value = 1; }
    if (n == 4) {value = 2; }
    if (n == 8) {value = 3; }
    return value;
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
    
    set_control(0);
    set_control(0);

    while(1) {
        PROCESS_YIELD();
        if(ev == tcpip_event) {
            udp_receive_data();
            set_control(recv); 
            if (((recv & 0b10000000) >> MOVE_CONTROL) == 0) {
                printf("ctimer set for %d\n", log2(recv));
                printf("callback_recv: %d, wert: %d\n", callback_recv, *callback_recv);
                ctimer_set(&button_timer[log2(recv)], CLOCK_SECOND*0.1, unset_control, &recv);
            }
            recv = 0;
         }  
    }  
    PROCESS_END();
}
/*--------------------------------------------------------------------------------------*/
PROCESS_THREAD(button_hold, ev, data) {
    PROCESS_BEGIN();
    while (1) {
        PROCESS_YIELD();
        //FEHLFUNKTION BEI TIMER
        printf("ende yield\n");
        /*if (etimer_expired(&button_timer[0])) {
            set_control((1 << CROSS));
            printf("Timer %d expierd\n", CROSS);
        }
        if (etimer_expired(&button_timer[1])) {
            set_control((1 << CIRCLE));
            printf("Timer %d expierd\n", CROSS);
        }
        if (etimer_expired(&button_timer[2])) {
            set_control((1 << TRIANGLE));
            printf("Timer %d expierd\n", CROSS);
        }
        if (etimer_expired(&button_timer[3])) {
            set_control((1 << SQUARE));
            printf("Timer %d expierd\n", CROSS);
        }*/
    }
    PROCESS_END();
}
/*--------------------------------------------------------------------------------------*/
