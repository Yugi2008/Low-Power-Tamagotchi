/*
 * Joystick.c
 *
 *  Created on: Mar 29, 2023
 *      Author: Antonio Dominguez
 */
#include <HAL/Joystick.h>

#define UP_THRESHOLD 12000
#define DOWN_THRESHOLD 3000
#define LEFT_THRESHOLD 3000
#define RIGHT_THRESHOLD 12000

enum _JoystickDebounceState {MIDDLE, UP, DOWN, RIGHT, LEFT};
typedef enum _JoystickDebounceState JoystickDebounceState;

static volatile bool JoystickX_modified;

void ADC14_IRQHandler(){
    if(ADC14_getEnabledInterruptStatus() && ADC_INT0){
        JoystickX_modified = true;
    }
    ADC14_clearInterruptFlag(ADC_INT0);

}

void initADC() {
    ADC14_enableModule();

    ADC14_initModule(ADC_CLOCKSOURCE_SYSOSC,
                     ADC_PREDIVIDER_64,
                     ADC_DIVIDER_8,
                      0
                     );

    // This configures the ADC to store output results
    // in ADC_MEM0 for joystick X.
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);

    ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192, ADC_PULSE_WIDTH_192);

    // This configures the ADC in manual conversion mode
    // Software will start each conversion.
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

}


void startADC() {
   // Starts the ADC with the first conversion
   // in repeat-mode, subsequent conversions run automatically
   ADC14_enableConversion();
   ADC14_toggleConversionTrigger();
}


// Interfacing the Joystick with ADC (making the proper connections in software)
void initJoyStick() {

    // This configures ADC_MEM0 to store the result from
    // input channel A15 (Joystick X), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM0,
                                  ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                  ADC_INPUT_A15,                 // joystick X
                                  ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input
    // A15 is multiplexed on GPIO port P6 pin PIN0

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                               GPIO_PIN0,
                                               GPIO_TERTIARY_MODULE_FUNCTION);


    // This configures ADC_MEM1 to store the result from
    // input channel A9 (Joystick y), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM1,
                                  ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                  ADC_INPUT_A9,                 // joystick y
                                  ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input
    // A15 is multiplexed on GPIO port P4 pin PIN4

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN4,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    //setup the interrupt for the ADC
    ADC14_clearInterruptFlag(ADC_INT0);
    ADC14_enableInterrupt(ADC_INT0);

    Interrupt_enableInterrupt(INT_ADC14);
}





/**
 * Constructs a Joystick
 * Initializes the output FSMs.
 *
 *
 * @return a constructed Joystick with debouncing and output FSMs initialized
 */

Joystick Joystick_construct()
{
    // The Joystick object which will be returned at the end of construction
    Joystick Joystick;

    initADC();
    initJoyStick();
    startADC();




    // Initialize all buffered outputs of the Joystick
//    Joystick.pushState = RELEASED;
//    Joystick.isTapped = false;

    // Return the constructed Joystick object to the user
    return Joystick;
}


/**
 * Refreshes the input of the provided Joystick by polling its x and y channel
 * pin value and advancing the debouncing FSM by one step.
 *
 * @param Joystick_p:   The Joystick object to refresh
 */
void Joystick_refresh(Joystick* joystick_p)
{
    if(JoystickX_modified){
        joystick_p->x = ADC14_getResult(ADC_MEM0);
        JoystickX_modified = false;
    }


    joystick_p->y =ADC14_getResult(ADC_MEM1);

    static JoystickDebounceState state = MIDDLE;
    joystick_p->isTappedUp = false;
    joystick_p->isTappedDown = false;
    joystick_p->isTappedLeft = false;
    joystick_p->isTappedRight = false;
    switch(state){
    case MIDDLE:
        if(joystick_p->y > UP_THRESHOLD){
            state = UP;
            joystick_p->isTappedUp = true;
        }
        if(joystick_p->y < DOWN_THRESHOLD){
            state = DOWN;
            joystick_p->isTappedDown = true;
        }
        if(joystick_p->x > RIGHT_THRESHOLD){
            state = RIGHT;
            joystick_p->isTappedRight = true;
        }
        if(joystick_p->x < LEFT_THRESHOLD){
            state = LEFT;
            joystick_p->isTappedLeft = true;
        }
        break;

    case UP:
        if(joystick_p->y < UP_THRESHOLD){
            state = MIDDLE;
            joystick_p->isTappedUp = false;

        }
        break;
    case DOWN:
        if(joystick_p->y > DOWN_THRESHOLD){
            state = MIDDLE;
            joystick_p->isTappedDown = false;

        }
        break;
    case RIGHT:
            if(joystick_p->x < RIGHT_THRESHOLD){
                state = MIDDLE;
                joystick_p->isTappedRight = false;

            }
     break;
    case LEFT:
        if(joystick_p->x > LEFT_THRESHOLD){
            state = MIDDLE;
            joystick_p->isTappedLeft = false;

         }
    break;


    }

}


/** Given a Joystick, determines if the switch is currently pressed to left */
bool Joystick_isPressedToLeft(Joystick* joystick_p){

    return (joystick_p->x < LEFT_THRESHOLD);


}

/** Given a Joystick, determines if the switch is currently pressed to left */
bool Joystick_isPressedToRight(Joystick* joystick_p){

    return (joystick_p->x > RIGHT_THRESHOLD);


}


/** Given a Joystick, determines if the switch is currently pressed up */
bool Joystick_isPressedUp(Joystick* joystick_p){

    return (joystick_p->y > UP_THRESHOLD);


}

/** Given a Joystick, determines if the switch is currently pressed down */
bool Joystick_isPressedDown(Joystick* joystick_p){

    return (joystick_p->y < DOWN_THRESHOLD);


}




/** Given a Joystick, determines if it was "tapped" to left - middle to left */
 bool Joystick_isTappedUp(Joystick* joystick_p){
    return (joystick_p->isTappedUp);

 }

 /** Given a Joystick, determines if it was "tapped" to left - middle to left */
  bool Joystick_isTappedDown(Joystick* joystick_p){
     return (joystick_p->isTappedDown);

  }



  /** Given a Joystick, determines if it was "tapped" to left - middle to left */
   bool Joystick_isTappedRight(Joystick* joystick_p){
      return (joystick_p->isTappedRight);

   }

   /** Given a Joystick, determines if it was "tapped" to left - middle to left */
    bool Joystick_isTappedLeft(Joystick* joystick_p){
       return (joystick_p->isTappedLeft);

    }

