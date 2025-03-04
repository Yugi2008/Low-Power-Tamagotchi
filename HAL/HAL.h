/*
 * HAL.h
 * Created on: Mar 29, 2023
 * Author: Antonio Dominguez
 */

#ifndef HAL_HAL_H_
#define HAL_HAL_H_

#include <HAL/Button.h>
#include <HAL/LED.h>
#include <HAL/Timer.h>
#include <HAL/Graphics.h>
#include <HAL/Joystick.h>

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/**============================================================================
 * The main HAL struct. This struct encapsulates all of the other input structs
 * in this application as individual members. This includes all LEDs, all
 * Buttons, one HWTimer from which all software timers should reference, the
 * Joystick, and any other peripherals with which you wish to interface.
 * ============================================================================
 * USAGE WARNINGS
 * ============================================================================
 * YOU SHOULD HAVE EXACTLY ONE HAL STRUCT IN YOUR ENTIRE PROJECT. We recommend
 * you put this struct inside of a main [Application] object or in the main
 * function so that every single function in your application has access to the
 * main inputs and outputs which interface with the hardware on the MSP432.
 */
struct _HAL
{

    // Graphics - LCD control
    GFX gfx;
};
typedef struct _HAL HAL;

// Constructs an HAL object by calling the constructor of each individual member
HAL HAL_construct();

// Refreshes all necessary inputs in the HAL
void HAL_refresh(HAL* hal_p);

#endif /* HAL_HAL_H_ */
