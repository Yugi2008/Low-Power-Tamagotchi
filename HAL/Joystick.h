/*
 * Joystick.h
 *
 *  Created on: Mar 29, 2023
 *      Author: Antonio Dominguez
 */

#ifndef HAL_JOYSTICK_H_
#define HAL_JOYSTICK_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

struct _Joystick
{
    uint_fast16_t x;
    uint_fast16_t y;


    bool isTappedUp;
    bool isTappedDown;
    bool isTappedRight;
    bool isTappedLeft;



};
typedef struct _Joystick Joystick;

/** Constructs a new Joystick object, given a valid port and pin. */
Joystick Joystick_construct();

/** Given a Joystick, determines if the switch is currently pressed to left */
bool Joystick_isPressedToLeft(Joystick* Joystick_p);

/** Given a Joystick, determines if the switch is currently pressed to right */
bool Joystick_isPressedToRight(Joystick* Joystick);

/** Given a Joystick, determines if the switch is currently pressed to right */
bool Joystick_isPressedUp(Joystick* Joystick);

/** Given a Joystick, determines if the switch is currently pressed to right */
bool Joystick_isPressedDown(Joystick* Joystick);


/** Given a Joystick, determines if it was "tapped" to left - middle to left */
bool Joystick_isTappedUp(Joystick* Joystick);

/** Given a Joystick, determines if it was "tapped" to Right - middle to Right */
bool Joystick_isTappedDown(Joystick* Joystick);

/** Given a Joystick, determines if it was "tapped" to left - middle to left */
bool Joystick_isTappedRight(Joystick* Joystick);

/** Given a Joystick, determines if it was "tapped" to Right - middle to Right */
bool Joystick_isTappedLeft(Joystick* Joystick);


/** Refreshes this Joystick so the Joystick FSM now has new outputs to interpret */
void Joystick_refresh(Joystick* Joystick);




#endif /* HAL_JOYSTICK_H_ */
