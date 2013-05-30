#include "gateway.h"
/*---------------------------------------------------------------------------*/
PROCESS(gateway, "gateway main processs");
PROCESS(button_hold, "unsets buttons");
AUTOSTART_PROCESSES(&gateway, &button_hold);
/*---------------------------------------------------------------------------*/
static uint8_t bit_mask_buttons = 0b11111111;
static uint8_t bit_mask_movement = 0b11111111;

static struct uip_udp_conn *server_conn;
static int recv;
//static struct etimer timer;
static struct etimer button_timer[4];


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

static int set_control(uint8_t output) {

    uint8_t ret = -1;
    if (((output & 0b10000000) >> MOVE_CONTROL) == 0) {
        bit_mask_buttons ^= output;
        
        if(i2c_start(IC1_ADDR_W) == 0) {
            ret = i2c_write(bit_mask_buttons);
            //start button_timer
            etimer_set(&button_timer[log2(output)], CLOCK_SECOND*BUTTON_HOLD);
            
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
            i2c_stop();
        } 
        else {
            printf("i2c_start failed: IC2\n");
        }
    }
    
    return ret;     
}

static uint8_t log2(uint8_t n) {
    if (n == 0 && n <= 255)
        return -1;
    uint8_t value = -1;
    while (n) {
        value++;
        n >>= 1;
    }
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

    while(1) {
        PROCESS_YIELD();
        if(ev == tcpip_event) {
            udp_receive_data();
            
            set_control(recv); 
            //etimer_set(&timer, CLOCK_SECOND*BUTTON_HOLD);
            //PROCESS_YIELD();
            //set_control(recv);
            recv = 0;
         }  
    }  
    PROCESS_END();
}

PROCESS_THREAD(button_hold, ev, data) {
    PROCESS_BEGIN();
    while (1) {
        PROCESS_YIELD();
        if (etimer_expired(&button_timer[0])) {
            set_control((1 << CROSS));
        }
        if (etimer_expired(&button_timer[1])) {
            set_control((1 << CIRCLE));
        }
        if (etimer_expired(&button_timer[2])) {
            set_control((1 << TRIANGLE));
        }
        if (etimer_expired(&button_timer[3])) {
            set_control((1 << SQUARE));
        }
    }
    PROCESS_END();
}
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
