/*
 * tamagotchi_app.h
 *
 * Created on: Mar 29, 2023
 * Author: Antonio Dominguez
 */

#ifndef TAMAGOTCHI_APP_H_
#define TAMAGOTCHI_APP_H_

#include <HAL/HAL.h>
#include <HAL/Graphics.h>
#include <HAL/Timer.h>

#define TITLE_SCREEN_WAIT   3000  // 3 seconds
#define DECREASE_INT        3000  // 3 seconds

enum _GameState
{
    TITLE_SCREEN, INSTRUCTIONS_SCREEN, GAME_SCREEN, GAME_OVER
};
typedef enum _GameState GameState;

enum _GameSpot
{
    CHILD, TEEN, ADULT
};
typedef enum _GameSpot GameSpot;

/**
 * The top-level application object, initialized in main() and
 * passed around to most functions. It holds the state variables
 * for the Tamagotchi game.
 */
struct _TamagotchiApp
{
    GameSpot gamespot;
    GameState state;  // Determines which screen is currently shown
    SWTimer timer;    // General-purpose timer for screen transitions
    SWTimer Dtimer;
    int age;          // Age display
    int ageSpot;
    int energy;
    int happiness;
    int spot;
    int begin;        // Starting position
    int end;
    int spotloc;
    bool needRemoved;
    int waitToPass;
    int movements;    // Total movements
};
typedef struct _TamagotchiApp TamagotchiApp;

// Constructor for the application
TamagotchiApp Tamagotchi_construct(HAL* hal_p);
void main_loop(TamagotchiApp* app_p, Graphics_Context *g_sContext_p, Joystick *joystick_p, HAL* hal_p);

// Callback functions for each state of the game
void Tamagotchi_handleTitleScreen(TamagotchiApp* app_p, HAL* hal_p);
void Tamagotchi_showTitleScreen(GFX* gfx_p);
void Tamagotchi_showInstructionsScreen(TamagotchiApp* app_p, GFX* gfx_p);
void Tamagotchi_handleGameScreen(TamagotchiApp* app_p, GFX* gfx_p, HAL* hal_p);
void Tamagotchi_showGameScreen(TamagotchiApp* app_p, GFX* gfx_p);
void Tamagotchi_GameMovement(TamagotchiApp* app_p, GFX* gfx_p);
void Tamagotchi_GAMEFSM(TamagotchiApp* app_p, GFX* gfx_p, Joystick *joystick_p);
void Tamagotchi_showEndScreen(TamagotchiApp* app_p, GFX* gfx_p);
void Tamagotchi_Tamagotchi(TamagotchiApp* app_p, GFX* gfx_p);
void Tamagotchi_movingLeft(TamagotchiApp* app_p, GFX* gfx_p, Joystick *joystick_p);
void Tamagotchi_movingRight(TamagotchiApp* app_p, GFX* gfx_p, Joystick *joystick_p);
void Tamagotchi_childState(TamagotchiApp* app_p, GFX* gfx_p);
void Tamagotchi_teenState(TamagotchiApp* app_p, GFX* gfx_p);
void Tamagotchi_adultState(TamagotchiApp* app_p, GFX* gfx_p);

#endif /* TAMAGOTCHI_APP_H_ */
