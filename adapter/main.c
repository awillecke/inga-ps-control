#include "main.h"

//ablage der daten für controller interface
physicalButtonList_t buttonList;

//i2cdatamit Werten füllen, die der Master auslesen und ändern kann
i2cdata[CONTROL]=0;
i2cdata[MOVEMENT]=0;

uint8_t* buttonArray[NUMBER_OF_INPUTS];
uint8_t EEMEM buttonMapArray[NUMBER_OF_INPUTS];
uint8_t buttonLockingArray[NUMBER_OF_INPUTS];
uint8_t EEMEM buttonLockingStorage[NUMBER_OF_INPUTS];
enum lockState{cantLock, unlocked, lockingPress, locked, unlockingPress};


// Read digital pin macro.  Returns 1 if the pin is LOW, 0 if the pin is HIGH
//  This is because all our digital pins are pulled up high
#define readPin(pin, pinNumber) (!( (pin) & (1 << pinNumber) ))

void readButtons(void){
    //aus den 2 bytes auswerten	
	buttonList.startOn = readPin(START_PIN, START);
	buttonList.circleOn = readPin(CIRCLE_PIN, CIRCLE);//(i2cdata[CONTROL] & 0b00000001)
	buttonList.triangleOn = readPin(TRIANGLE_PIN, TRIANGLE);
	buttonList.squareOn = readPin(SQUARE_PIN, SQUARE);
	buttonList.crossOn = readPin(CROSS_PIN, CROSS);
	buttonList.selectOn = readPin(SELECT_PIN, SELECT);
	
	buttonList.dpadDownOn = readPin(DPADDOWN_PIN, DPADDOWN);//(i2cdata[MOVEMENT] & 0b00000001)
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
	//applyRapidFire();
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
	dataToSend.selectOn = btnList.selectOn;
	dataToSend.squareOn = btnList.squareOn;
	dataToSend.startOn = btnList.startOn;
	dataToSend.triangleOn = btnList.triangleOn;
	return dataToSend;
}

int main (void) {
    
    //TWI als Slave mit Adresse slaveadr starten
    init_twi_slave(SLAVE_ADRESSE);
    
  	startPS2Communication();
  	
  	
  	buttonArray[0] = &buttonList.startOn;
	buttonArray[1] = &buttonList.circleOn;
	buttonArray[2] = &buttonList.triangleOn;
	buttonArray[3] = &buttonList.squareOn;
	buttonArray[4] = &buttonList.crossOn;
	buttonArray[5] = &buttonList.selectOn;
	
	buttonArray[6] = &buttonList.dpadDownOn;
	buttonArray[7] = &buttonList.dpadRightOn;
	buttonArray[8] = &buttonList.dpadLeftOn;
	buttonArray[9] = &buttonList.dpadUpOn;
	
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

    while(1) {
        // Next, we need to read the physical buttons and store them into an abstract
		//  representation of what physical buttons are being held
		readButtons();
		
		// After we know what physical buttons are being held, we can map those buttons
		//  to the output buttons we want to send
		transformButtonsPressed();
		
		// Finally, send the data
		dataForController_t dataToSend = buttonListToDataForController(buttonList);
		sendPS2Data(dataToSend);
        //_delay_ms(500);
    } //end.while
} //end.main
