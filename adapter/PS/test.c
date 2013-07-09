#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include "dataForController_t.h"
#include "physicalButtonList_t.h"
#include "PS2/PS2interface.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
// Divide this by 10 to get the number of ms to wait while the programming
//  button is down until canceling out of programming mode


// buttonList is a struct that holds the information about what
//  actual buttons are being pressed on the controller.
//  It gets updated by ReadButtons()
physicalButtonList_t buttonList;

// buttonArray is an array of uint8_t's representing
//  an ordered list of the buttons physically pressed
//  on the controller.  The order isn't important, so
//  long as it doesn't change.  This gets filled by readButtons()
uint8_t* buttonArray[NUMBER_OF_INPUTS];
// buttonMapArray is an array that maps inputs to outputs.
//  Each input is represented by an element in the array, and the
//  value at that array index is what logical button it outputs
uint8_t EEMEM buttonMapArray[NUMBER_OF_INPUTS];
// buttonLockingArray is an array that is used to apply a locking
//  feature to specific buttons.  If buttonLockingArray[buttonNumber]
//  is in any of the lockStates other than cantLock, pressing and releasing
//  that button will cycle it through the lockStates
uint8_t buttonLockingArray[NUMBER_OF_INPUTS];
uint8_t EEMEM buttonLockingStorage[NUMBER_OF_INPUTS];
enum lockState{cantLock, unlocked, lockingPress, locked, unlockingPress};
	
// rapdiFireArray is an array that holds values for how fast buttons get pressed.
//  Rapid fire works by decrementing the values in the buttons every main loop
//  and 'pressing' the button only when the value reaches a certain cutoff, and 
//  resetting when the value reaches zero.
uint16_t rapidFireArray[NUMBER_OF_INPUTS];
uint16_t EEMEM rapidFireResetArray[NUMBER_OF_INPUTS]; /* = {10, 20, 30, 40, 50, 60,
											 70, 80, 90, 100, 110, 120,
											 130, 140, 1500, 1600, 1700, 1800,
											 1900, 2000, 2100, 2200, 2300, 2400,
											 2500, 2600, 2700, 2800, 2900, 3000,
											 3100, 3200, 3300, 3400, 3500, 3600};*/

// Read digital pin macro.  Returns 1 if the pin is LOW, 0 if the pin is HIGH
//  This is because all our digital pins are pulled up high
#define readPin(pin, pinNumber) (!( (pin) & (1 << pinNumber) ))

void readButtons(void){
    //aus den 2 bytes auswerten	
	buttonList.startOn = readPin(START_PIN, START);
	buttonList.circleOn = readPin(CIRCLE_PIN, CIRCLE);
	buttonList.triangleOn = readPin(TRIANGLE_PIN, TRIANGLE);
	buttonList.squareOn = readPin(SQUARE_PIN, SQUARE);
	buttonList.crossOn = readPin(CROSS_PIN, CROSS);
	buttonList.menuOn = readPin(MENU_PIN, MENU);
	buttonList.selectOn = readPin(SELECT_PIN, SELECT);
	
	buttonList.dpadDownOn = readPin(DPADDOWN_PIN, DPADDOWN);
	buttonList.dpadRightOn = readPin(DPADRIGHT_PIN, DPADRIGHT);
	buttonList.dpadLeftOn = readPin(DPADLEFT_PIN, DPADLEFT);
	buttonList.dpadUpOn = readPin(DPADUP_PIN, DPADUP);
}

// This function uses the data from the button presses
//  to cycle through the locking state of the buttons
void setLockingArray(void){
	for (uint8_t i = 0; i < NUMBER_OF_INPUTS; i++){
		// If the button is pressed
		if (*buttonArray[i] == 1){
			if (buttonLockingArray[i] == unlocked)
				buttonLockingArray[i] = lockingPress;
			if (buttonLockingArray[i] == locked)
				buttonLockingArray[i] = unlockingPress;
		}
		// If the button is unpressed
		else{
			if (buttonLockingArray[i] == lockingPress)
				buttonLockingArray[i] = locked;
			if (buttonLockingArray[i] == unlockingPress)
				buttonLockingArray[i] = unlocked;
		}
	}
}	

// This function applies a map to the data, turning the data from
//  the physical buttons to virtual buttons
void transformButtonsPressed(void){
	// This stores a temporary copy of the 
	uint8_t newButtonArray[NUMBER_OF_INPUTS];
	// Fill it with zeroes to give us a blank slate
	for (uint8_t i = 0; i < NUMBER_OF_INPUTS; i++){
			newButtonArray[i] = 0;
	}
	
	// Use the actual button press data to control the 
	//  locking behavior
	setLockingArray();	
	// Now we can trash the actual button data with our
	//  locking button data
	for (uint8_t i = 0; i < NUMBER_OF_INPUTS; i++){
		if (buttonLockingArray[i] == locked)
			*buttonArray[i] = 1;
	}

	
	// We've taken care of locking. Now for rapid fire.
	applyRapidFire();
	// Next, take the physical button presses, convert them
	//  through the mapping function, and write the new
	//  button press data with the mapped button data
	//  Note: buttonArray is an array of pointers to the actual press data
	for (uint8_t i = 0; i < NUMBER_OF_INPUTS; i++){
		newButtonArray[eeprom_read_byte(&buttonMapArray[i])] |= *buttonArray[i];
	}
	// Finally, we copy the new button mapping back into
	//  the buttonArray, replacing the physical button presses
	//  with the abstracted ones
	for (uint8_t i = 0; i < NUMBER_OF_INPUTS; i++){
		*buttonArray[i] = newButtonArray[i];
	}
}

// Reads the buttons and returns the number of the first button it
//  finds that is pressed down.
//  Returns -1 if no button is pressed
int8_t readOneButtonPress(void){
	readButtons();
	for (int8_t i = 0; i < NUMBER_OF_DIGITAL_BUTTONS; i++){
		if (*buttonArray[i] == 1)
			return i;
	}
	// If we're here, no button was pressed, so let the caller know that
	return -1;
}

dataForController_t buttonListToDataForController(physicalButtonList_t btnList){
	dataForController_t dataToSend;
	dataToSend.circleOn = btnList.circleOn;
	dataToSend.crossOn  = btnList.crossOn;
	dataToSend.dpadDownOn = btnList.dpadDownOn;
	dataToSend.dpadLeftOn = btnList.dpadLeftOn;
	dataToSend.dpadRightOn = btnList.dpadRightOn;
	dataToSend.dpadUpOn = btnList.dpadUpOn;
	dataToSend.homeOn = btnList.menuOn;
	dataToSend.l1On = btnList.l1On;
	dataToSend.l2On = btnList.l2On;
	dataToSend.l3On = btnList.l3On;
	dataToSend.leftStickX = btnList.leftStickX;
	dataToSend.leftStickY = btnList.leftStickY;
	dataToSend.r1On = btnList.r1On;
	dataToSend.r2On = btnList.r2On;
	dataToSend.r3On = btnList.r3On;
	dataToSend.rightStickX = btnList.rightStickX;
	dataToSend.rightStickY = btnList.rightStickY;
	dataToSend.selectOn = btnList.selectOn;
	dataToSend.squareOn = btnList.squareOn;
	dataToSend.startOn = btnList.startOn;
	dataToSend.triangleOn = btnList.triangleOn;
	return dataToSend;
}

int main(void) {
	// First things first.  If we're holding certain buttons on plug-in,
	// then reset
	readButtons();
	wdt_disable();
	// set for 16 MHz clock
	CPU_PRESCALE(0);

	startPS2Communication();
	
	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
//	_delay_ms(500);
	buttonArray[3] = &buttonList.startOn;
	buttonArray[4] = &buttonList.circleOn;
	buttonArray[5] = &buttonList.triangleOn;
	buttonArray[6] = &buttonList.squareOn;
	buttonArray[7] = &buttonList.crossOn;
	buttonArray[12] = &buttonList.selectOn;
	
	buttonArray[13] = &buttonList.dpadDownOn;
	buttonArray[14] = &buttonList.dpadRightOn;
	buttonArray[15] = &buttonList.dpadLeftOn;
	buttonArray[16] = &buttonList.dpadUpOn;
	

	// Check to make sure our stored EEPROM variables make sense
	for (uint8_t i = 0; i < NUMBER_OF_INPUTS; i++){
		// First, check the button mapping array
		if (eeprom_read_byte(&buttonMapArray[i]) >= NUMBER_OF_INPUTS)
			eeprom_write_byte(&buttonMapArray[i], i);// = i;
			
		// Next, check the button locking data, and load in the stored locking data
		if (eeprom_read_byte(&buttonLockingStorage[i]) >= 5)
			eeprom_write_byte(&buttonLockingStorage[i], cantLock);
		else
			buttonLockingArray[i] = eeprom_read_byte(&buttonLockingStorage[i]);
		
		// Finally, check the rapid fire data
		if (eeprom_read_word(&rapidFireResetArray[i]) > RAPID_FIRE_MAX)
			eeprom_write_word(&rapidFireResetArray[i], RAPID_FIRE_MIN);
	}
		
	while (1) {
		// Next, we need to read the physical buttons and store them into an abstract
		//  representation of what physical buttons are being held
		readButtons();
		
		// After we know what physical buttons are being held, we can map those buttons
		//  to the output buttons we want to send
		transformButtonsPressed();
		
		// Finally, send the data
		dataForController_t dataToSend = buttonListToDataForController(buttonList);
		sendPS2Data(dataToSend);
		
	}
}
