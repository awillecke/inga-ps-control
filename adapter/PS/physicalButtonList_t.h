/*
 * typedefs.h
 *
 * Created: 12/13/2011 2:26:51 PM
 *  Author: Alan Chatham
 *
 *	This file contains all the typedef'ed structs we're using to store button data
 */ 


#ifndef PHYSICAL_BUTTON_LIST_T
#define PHYSICAL_BUTTON_LIST_T

// How many total inputs do we have?
#define NUMBER_OF_INPUTS 10
#define NUMBER_OF_DIGITAL_BUTTONS 10

// This one is an abstract representation of what physical buttons we have
//  on the controller itself
typedef struct physicalButtonList_t{
	uint8_t startOn;
	uint8_t triangleOn;
	uint8_t circleOn;
	uint8_t squareOn;
	uint8_t crossOn;
	uint8_t selectOn;
	uint8_t dpadDownOn;
	uint8_t dpadRightOn;
	uint8_t dpadLeftOn;
	uint8_t dpadUpOn;
	
} physicalButtonList_t;
#endif // PHYSICAL_BUTTON_LIST_T
