/*
 * HAL.c
 *
 * Created on: Mar 29, 2023
 * Author: Antonio Dominguez
 */

#include <HAL/HAL.h>

/**
 * Constructs a new HAL object. The HAL constructor should simply call the constructors of each
 * of its sub-members with the proper inputs.
 *
 * @return a properly constructed HAL object.
 */
HAL HAL_construct()
{
    // The HAL object which will be returned at the end of construction
    HAL hal;

    // Initialize all LEDs by calling their constructors with correctly-defined arguments.

    // Initialize the LCD by calling its constructor with user-defined foreground and background colors.
    hal.gfx = GFX_construct(GRAPHICS_COLOR_BLACK, GRAPHICS_COLOR_WHITE);

    // Once we have finished building the HAL, return the completed struct.
    return hal;
}
