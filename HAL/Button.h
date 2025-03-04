/*
 * Button.h
 */

#ifndef HAL_BUTTON_H_
#define HAL_BUTTON_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// This structure holds the tapping status of all the buttons in our kit
typedef struct{
    bool LB1tapped;
    bool LB2tapped;
    bool BB1tapped;
    bool BB2tapped;
    bool JSBtapped;
} buttons_t;

// This function initializes all buttons
void initButtons();

// This function updates the tapping status of all the buttons
buttons_t updateButtons();

#endif /* HAL_BUTTON_H_ */
