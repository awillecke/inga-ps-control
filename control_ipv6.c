#include "control.h"
#include "contiki-net.h"
#include "settings.h"

#include "node-id.h"

#include "net/uip-debug.h"
#define DEBUG DEBUG_PRINT
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

PROCESS(default_app_process, "Controller process");
AUTOSTART_PROCESSES(&default_app_process);


static struct etimer timer;

static int16_t x = 0;
static int count = 0;

/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *client_conn;

/*---------------------------------------------------------------------------*/
static void udp_send_data(void) {
    uip_udp_packet_send(client_conn, (char *)CONTROL, 2/*strlen(buf)*/);
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------*/
static void set_connection_address(uip_ipaddr_t *ipaddr) {
    //fe80::f00:22ff:fe33:4455 is node 13
    if (node_id == 12)
        uip_ip6addr(ipaddr,0xfe80,0,0,0,0xf00,0x22ff,0xfe33,0x4455);
    //fe80::e00:22ff:fe33:4455 is node 12
    else if (node_id == 13)
        uip_ip6addr(ipaddr,0xfe80,0,0,0,0xe00,0x22ff,0xfe33,0x4455);
     else
        printf("No destiniation!");
}

static void print_control() {
        #if CONTROL == 0 
        printf("CROSS");  
        #elif CONTROL == 1
        printf("CIRCLE");
        #elif CONTROL == 2
        printf("TRIANGLE");
        #elif CONTROL == 3
        printf("SQUARE"); 
        #endif
}
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
    etimer_set(&timer, CLOCK_SECOND * 0.05);
    
    printf("Note configured as ");
    print_control();
    printf("\n");
    
    while (1) { 
        PROCESS_YIELD();
        if ( etimer_expired(&timer) ) {
            etimer_set(&timer, CLOCK_SECOND*0.05);
            x = acc_sensor.value(ACC_Y_RAW);
            if (x > 380) { 
                //send
                udp_send_data();
                etimer_set(&timer, CLOCK_SECOND*0.3);
                count++;
            } 
        }
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
