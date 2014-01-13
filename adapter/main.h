#include <stdint.h> 	    //definiert den Datentyp uint8_t
#include <stdlib.h>         //nötig für Zahlumwandlung mit itoa
#include <util/twi.h> 	    //enthält z.B. die Bezeichnungen für die Statuscodes in TWSR
#include <util/delay.h>
#include <avr/interrupt.h>  //dient zur Behandlung der Interrupts
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include "TWI/twislave.h"
#include "PS/dataForController_t.h"
#include "PS/physicalButtonList_t.h"
#include "PS/PS2interface.h"

#define BAUD 9600 //Baudrate
#define SLAVE_ADRESSE 0x50 //Die Slave-Adresse

#define CONTROL 0
#define MOVEMENT 1

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))




