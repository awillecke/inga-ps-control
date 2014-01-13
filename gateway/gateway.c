#include "gateway.h"
/*---------------------------------------------------------------------------*/
PROCESS(gateway, "gateway main processs");
AUTOSTART_PROCESSES(&gateway);
/*---------------------------------------------------------------------------*/
static uint8_t bit_mask_buttons = 0b11111111;
static uint8_t bit_mask_movement = 0b11111111;

static uint8_t recv = 0;

static struct uip_udp_conn *server_conn;
static struct ctimer button_timer[4];
//static void (*unset_control) = set_control;
/*--------------------------------------------------------------------------------------*/
static void udp_receive_data(void) {
    if(uip_newdata()) {
        recv = (uint8_t)atoi((char *)uip_appdata);
        //printf("Empfange Paket: %d\n", recv);
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
static void set_control(void *output_ptr) {

    uint8_t *tmp = (uint8_t *) output_ptr;
    uint8_t output = *tmp;

    if (((output & 0b10000000) >> MOVE_CONTROL) == 0) {
        bit_mask_buttons ^= output;
        
        if(i2c_start(IC1_ADDR_W) == 0) {
            i2c_write(bit_mask_buttons);
            printf("set button: %d\n", bit_mask_buttons);//output);  
            i2c_stop();
        } 
        else {
            printf("i2c_start failed: IC1\n");
        }
    }
    else {
        bit_mask_movement ^= (output & 0b01111111);
        
        if(i2c_start(IC2_ADDR_W) == 0) {
            i2c_write(bit_mask_movement);
            printf("set movement: %d\n", bit_mask_movement);
            i2c_stop();
        } 
        else {
            printf("i2c_start failed: IC2\n");
        }
    }   
    return;
}
/*--------------------------------------------------------------------------------------*/
static void unset_control(void *output_ptr) {

    uint8_t *tmp = (uint8_t *) output_ptr;
    uint8_t output = *tmp;
    
    if (((output & 0b10000000) >> MOVE_CONTROL) == 0) {
        bit_mask_buttons |= output;  // set bit number $output to unset button
        
        if(i2c_start(IC1_ADDR_W) == 0) {
            i2c_write(bit_mask_buttons);
            printf("unset button: %d\n", bit_mask_buttons);//output);  
            i2c_stop();
        } 
        else {
            printf("i2c_start failed: IC1\n");
        }
    }
    else {
        bit_mask_movement |= (output & 0b01111111);
        
        if(i2c_start(IC2_ADDR_W) == 0) {
            i2c_write(bit_mask_movement);
            printf("unset movement: %d\n", bit_mask_movement);
            i2c_stop();
        } 
        else {
            printf("i2c_start failed: IC2\n");
        }
    }
    
    return;  
}
/*--------------------------------------------------------------------------------------*/
static uint8_t log_2(uint8_t n) {
    if (n == 1) return 0;
    else if (n == 2) return 1;
    else if (n == 4) return 2;
    else if (n == 8) return 3;
    else return -1;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gateway, ev, data) {
  
    PROCESS_BEGIN();
    
    static uint8_t recv_vals[] = {1, 2, 4, 8};

    i2c_init();
    
    //set
    set_control(&recv);
    //unset
    set_control(&recv);
     
    print_local_addresses();

    server_conn = udp_new(NULL, UIP_HTONS(3003), NULL);
    udp_bind(server_conn, UIP_HTONS(3002));

    while(1) {
        PROCESS_YIELD();
        if(ev == tcpip_event) {
            udp_receive_data();
            
            //printf("recv: %d. &128: %d\n", recv, recv & 128);
            //printf("bla: %d\n", ((recv & 128) >> MOVE_CONTROL));
            
            // check, if movement-bit is set
            if (((recv & 0b10000000) >> MOVE_CONTROL) == 0) {
                //printf("ctimer set for %d\n", log_2(recv));
                //printf("callback_recv: %d, wert: %d\n", callback_recv, *callback_recv);
                set_control(&recv);
                ctimer_set(&button_timer[log_2(recv)], CLOCK_SECOND*0.05, unset_control, (void *) &recv_vals[log_2(recv)]);
            }
            else {
                // check received state
                if((recv & 0b01000000) == 0) {
                    // set
                    set_control(&recv);
                }
                else {
                    // unset
                    unset_control(&recv);
                }
            }
            recv = 0;
         }  
    }  
    PROCESS_END();
}
/*--------------------------------------------------------------------------------------*/
