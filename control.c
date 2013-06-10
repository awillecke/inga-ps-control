#include "control.h"

PROCESS(default_app_process, "Controler process");
AUTOSTART_PROCESSES(&default_app_process);

static struct etimer timer;

static int16_t y_acc = 0;
static uint8_t button_byte = 0;
#if MOVEMENT==1
static uint8_t movement_byte = 0;
static uint8_t old_movement = 0;
#endif
/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *client_conn;

/*---------------------------------------------------------------------------*/
static void udp_send_data(uint8_t data) {
    char buffer[5];
     itoa(data,buffer,10);
     uip_udp_packet_send(client_conn, buffer, 5/*strlen(buf)*/);
     printf("Sende Daten: %s\n", buffer);
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
static void set_connection_address(uip_ipaddr_t *ipaddr) {
    //alt
    //uip_ip6addr(ipaddr,0xfe80,0,0,0,0x3301,0x22ff,0xfe33,0x4455);
    // fe80::b01:22ff:fe33:4455
    uip_ip6addr(ipaddr,0xfe80,0,0,0,0xb01,0x22ff,0xfe33,0x4455);
}

#if MOVEMENT==1
static uint8_t get_movement() {
    
    uint8_t move = 0;
            
    if(i2c_start(IC1_ADDR_R) == 0) {
        i2c_read_nack(&move);
    }
    else {
        printf("i2c_start failed: IC1\n");
    }

    i2c_stop();
    
    return ~move;
}
/*---------------------------------------------------------------------------*/
static uint8_t log_2(uint8_t n) {
    if (n == 1) return 0;
    else if (n == 2)  return 1;
    else if (n == 4)  return 2;
    else if (n == 8)  return 3;
    else if (n == 16) return 4;
    else if (n == 32) return 5;
    else return -1;
}
#endif
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(default_app_process, ev, data) {

    uip_ipaddr_t ipaddr;
    
    PROCESS_BEGIN();
    
    set_connection_address(&ipaddr);
    
    /* new connection with remote host */
    client_conn = udp_new(&ipaddr, UIP_HTONS(3002), NULL);
    udp_bind(client_conn, UIP_HTONS(3003));

    PRINTF("Created a connection with the server ");
    PRINT6ADDR(&client_conn->ripaddr);
    PRINTF(" local/remote port %u/%u\n",
    UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));

    SENSORS_ACTIVATE(acc_sensor);
   
    button_byte |= (1 << CONTROL);    
    
    #if MOVEMENT==1
    printf("MOVEMENT Enabled\n");
    i2c_init();    
    #endif

    printf("CONTROL: %d Button_byte: %d\n", CONTROL, button_byte);    
    
    etimer_set(&timer, CLOCK_SECOND * 0.05);
    int stelle;
        
    while (1) { 
        PROCESS_YIELD();
        if ( etimer_expired(&timer) ) {
            
            #if MOVEMENT==1
            movement_byte = get_movement();
            
            //printf("mov_byte: %d\n", movement_byte);
/*            11011111
            11011011
            00000100
            stelle = 2
            new = 00110110
                  00000001   */         
            if (old_movement != movement_byte) {
            stelle = log_2(movement_byte ^ old_movement);
                if(((old_movement >> stelle) & 1) == 1 ) {
                    udp_send_data((old_movement ^ movement_byte) | (1 << MOVE_CONTROL) | (1 << STATE));
                }
                else {
                    udp_send_data((old_movement ^ movement_byte) | (1 << MOVE_CONTROL) | (0 << STATE));
                }
            }
            
            old_movement = movement_byte;
            
            #endif
            
            y_acc = acc_sensor.value(ACC_Y_RAW);
            
            if (y_acc > 380) { 
                printf("trigger");
                uint8_t tmp = 1 << CONTROL;
                udp_send_data(tmp);
            }
        }
        etimer_set(&timer, CLOCK_SECOND*0.05);
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
