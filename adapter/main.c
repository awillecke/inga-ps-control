#include "main.h"

int main (void) {
    
    //TWI als Slave mit Adresse slaveadr starten
    init_twi_slave(SLAVE_ADRESSE);

    //i2cdatamit Werten füllen, die der Master auslesen und ändern kann
    i2cdata[CONTROL]=0;
    i2cdata[MOVEMENT]=0;

    while(1) {
        _delay_ms(500);
    } //end.while
} //end.main
