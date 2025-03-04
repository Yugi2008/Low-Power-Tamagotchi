/* TI includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <LcdDriver/Crystalfontz128x128_ST7735.h>

/* Standard includes */
#include <stdio.h>

/* HAL includes */
#include "HAL/LED.h"
#include "HAL/Timer.h"
#include "HAL/Button.h"
#include "HAL/Joystick.h"
#include <HAL/HAL.h>
#include <tamagotchi_app.h>

#define BUFFER_SIZE 100

void initialize();
void initGraphics(Graphics_Context *g_sContext_p);
void main_loop(TamagotchiApp* app_p, Graphics_Context *g_sContext_p, Joystick *joystick_p, HAL* hal_p);
void sleep();

int main(void)
{
    initialize();

    /* Create a HAL instance and construct the Tamagotchi application */
    HAL hal = HAL_construct();
    TamagotchiApp app = Tamagotchi_construct(&hal);
    Graphics_Context g_sContext;
    initGraphics(&g_sContext);
    initLEDs();
    Joystick joystick = Joystick_construct();

    Tamagotchi_showTitleScreen(&hal.gfx);

    while (1) {
        sleep();
        Joystick_refresh(&joystick);
        main_loop(&app, &g_sContext, &joystick, &hal);
    }
}

/**
 * Constructs the Tamagotchi application and initializes its state variables.
 */
TamagotchiApp Tamagotchi_construct(HAL* hal_p)
{
    TamagotchiApp app;

    app.state = TITLE_SCREEN;
    app.timer = SWTimer_construct(TITLE_SCREEN_WAIT);
    SWTimer_start(&app.timer);
    app.Dtimer = SWTimer_construct(DECREASE_INT);

    app.age = 0;
    app.ageSpot = 0;
    app.energy = 5;
    app.happiness = 3;
    app.spot = 0;
    app.begin = 0;
    app.end = 0;
    app.waitToPass = 0;
    app.movements = 0;
    app.spotloc = 65;
    app.needRemoved = false;

    return app;
}

void sleep() {
    /* Indicate low-power mode with the Launchpad Green LED */
    TurnOn_LLG();
    PCM_gotoLPM0();
    TurnOff_LLG();
}

void main_loop(TamagotchiApp* app_p, Graphics_Context *g_sContext_p, Joystick *joystick_p, HAL* hal_p) {
    buttons_t buttons = updateButtons();

    /* Non-blocking code: Tapping the joystick push button toggles the BoosterPack Green LED */
    if (buttons.JSBtapped)
        Toggle_BLG();

    /* Additional button functionality: Check other buttons */
    if (buttons.LB1tapped || buttons.LB2tapped || buttons.BB2tapped)
        Toggle_BLB();

    switch (app_p->state)
    {
        case TITLE_SCREEN:
            Tamagotchi_handleTitleScreen(app_p, hal_p);
            break;

        case INSTRUCTIONS_SCREEN:
            if(buttons.BB1tapped){
                /* Reset state variables for a new game */
                app_p->age = 0;
                app_p->energy = 5;
                app_p->happiness = 3;
                app_p->spot = 0;
                app_p->begin = 0;
                app_p->end = 0;
                app_p->ageSpot = 0;
                app_p->movements = 0;
                app_p->spotloc = 65;
                Tamagotchi_showGameScreen(app_p, &hal_p->gfx);
                SWTimer_start(&app_p->Dtimer);
                app_p->state = GAME_SCREEN;
            }
            break;

        case GAME_SCREEN:
            Tamagotchi_GAMEFSM(app_p, &hal_p->gfx, joystick_p);
            Tamagotchi_handleGameScreen(app_p, &hal_p->gfx, hal_p);

            /* Increase energy when the pet is fed (BB1 pressed) */
            if(buttons.BB1tapped && app_p->energy < 5) {
                app_p->energy++;
                char buffer[BUFFER_SIZE];
                snprintf(buffer, BUFFER_SIZE, "%d", app_p->energy);
                GFX_print(&hal_p->gfx, buffer, 3, 11);
            }
            /* Transition to game over if energy and happiness are depleted */
            if (app_p->energy == 0 && app_p->happiness == 0){
                app_p->state = GAME_OVER;
            }
            break;

        case GAME_OVER:
            if (app_p->end == 0){
                Tamagotchi_showEndScreen(app_p, &hal_p->gfx);
                app_p->end++;
            }
            /* Return to instructions screen when BB1 is pressed */
            if(buttons.BB1tapped && app_p->energy < 5){
                app_p->state = INSTRUCTIONS_SCREEN;
                Tamagotchi_showInstructionsScreen(app_p, &hal_p->gfx);
            }
            break;
    }
}

void initialize()
{
    /* Stop watchdog timer and initialize system timing */
    WDT_A_hold(WDT_A_BASE);
    InitSystemTiming();

    initLEDs();
    initButtons();

    /* Additional initializations if necessary */
}

void initGraphics(Graphics_Context *g_sContext_p) {
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    Graphics_initContext(g_sContext_p, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setFont(g_sContext_p, &g_sFontFixed6x8);

    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(g_sContext_p, GRAPHICS_COLOR_WHITE);

    Graphics_clearDisplay(g_sContext_p);
}

void Tamagotchi_handleTitleScreen(TamagotchiApp* app_p, HAL* hal_p)
{
    if (SWTimer_expired(&app_p->timer))
    {
        app_p->state = INSTRUCTIONS_SCREEN;
        Tamagotchi_showInstructionsScreen(app_p, &hal_p->gfx);
    }
}

void Tamagotchi_showTitleScreen(GFX* gfx_p)
{
    GFX_clear(gfx_p);

    GFX_print(gfx_p, "Antonio Dominguez", 5, 2);
    GFX_print(gfx_p, "Low - Power", 7, 4);
    GFX_print(gfx_p, "Tamagotchi", 8, 3);
    GFX_print(gfx_p, "Your interrupt-", 10, 2);
    GFX_print(gfx_p, "driven virtual pet!", 11, 1);
}

void Tamagotchi_showInstructionsScreen(TamagotchiApp* app_p, GFX* gfx_p)
{
    GFX_clear(gfx_p);

    GFX_print(gfx_p, "Welcome to the", 1, 5);
    GFX_print(gfx_p, "wonderful world of", 2, 2);
    GFX_print(gfx_p, "TOMAGOTCHI!", 3, 5);
    GFX_print(gfx_p, "Take good care of", 5, 1);
    GFX_print(gfx_p, "your pet by feeding", 6, 1);
    GFX_print(gfx_p, "and playing with it.", 7, 1);
    GFX_print(gfx_p, "Watch as your", 8, 1);
    GFX_print(gfx_p, "Tamagotchi grows!", 9, 1);
    GFX_print(gfx_p, "Press BB1 to feed", 11, 1);
    GFX_print(gfx_p, "your pet. Tap the", 12, 1);
    GFX_print(gfx_p, "right to play. :)", 13, 1);
}

void Tamagotchi_handleGameScreen(TamagotchiApp* app_p, GFX* gfx_p, HAL* hal_p){
    char buffer[BUFFER_SIZE];
    if(SWTimer_expired(&app_p->Dtimer)){
        if(app_p->energy > 0){
            app_p->energy--;
            snprintf(buffer, BUFFER_SIZE, "%d", app_p->energy);
            GFX_print(gfx_p, buffer, 3, 11);
        }
        if(app_p->happiness > 0){
            app_p->happiness--;
            snprintf(buffer, BUFFER_SIZE, "%d", app_p->happiness);
            GFX_print(gfx_p, buffer, 5, 13);
        }
        app_p->age++;
        snprintf(buffer, BUFFER_SIZE, "%01d", app_p->age);
        GFX_print(gfx_p, buffer, 1, 10);
        SWTimer_start(&app_p->Dtimer);
   }
}

void Tamagotchi_showGameScreen(TamagotchiApp* app_p, GFX* gfx_p){
    char buffer[BUFFER_SIZE];

    GFX_clear(gfx_p);

    GFX_print(gfx_p, "Age: ", 1, 2);
    snprintf(buffer, BUFFER_SIZE, "%01d", app_p->age);
    GFX_print(gfx_p, buffer, 1, 10);

    GFX_print(gfx_p, "Energy:", 3, 2);
    snprintf(buffer, BUFFER_SIZE, "%d", app_p->energy);
    GFX_print(gfx_p, buffer, 3, 11);

    GFX_print(gfx_p, "Happiness:", 5, 2);
    snprintf(buffer, BUFFER_SIZE, "%d", app_p->happiness);
    GFX_print(gfx_p, buffer, 5, 13);

    static Graphics_Rectangle recOut = {120, 60, 10, 110};
    Graphics_drawRectangle(&gfx_p->context, &recOut);
}

void Tamagotchi_GAMEFSM(TamagotchiApp* app_p, GFX* gfx_p, Joystick *joystick_p){
    if(app_p->begin == 0){
        app_p->gamespot = CHILD;
        app_p->begin++;
    }
    Tamagotchi_movingLeft(app_p, gfx_p, joystick_p);
    Tamagotchi_movingRight(app_p, gfx_p, joystick_p);

    switch (app_p->gamespot)
    {
        case CHILD:
            Tamagotchi_childState(app_p, gfx_p);
            break;
        case TEEN:
            Tamagotchi_teenState(app_p, gfx_p);
            break;
        case ADULT:
            Tamagotchi_adultState(app_p, gfx_p);
            break;
    }
}

void Tamagotchi_showEndScreen(TamagotchiApp* app_p, GFX* gfx_p){
    char buffer[BUFFER_SIZE];
    GFX_clear(gfx_p);

    GFX_print(gfx_p, "Your Tamagotchi", 4, 3);
    GFX_print(gfx_p, "packed its bags", 5, 2);
    GFX_print(gfx_p, "and left...", 6, 5);

    GFX_print(gfx_p, "Age: ", 9, 5);
    snprintf(buffer, BUFFER_SIZE, "%01d", app_p->age);
    GFX_print(gfx_p, buffer, 9, 10);

    GFX_print(gfx_p, "Play Again? (BB1)", 13, 2);
}

void Tamagotchi_movingRight(TamagotchiApp* app_p, GFX* gfx_p, Joystick *joystick_p){
    char buffer[BUFFER_SIZE];
    if(Joystick_isTappedRight(joystick_p) && app_p->spot < 3 && app_p->energy > 0){
        app_p->spot++;
        app_p->spotloc += 10;
        app_p->needRemoved = true;
        app_p->movements++;
        if(app_p->happiness < 5){
            app_p->happiness++;
            snprintf(buffer, BUFFER_SIZE, "%d", app_p->happiness);
            GFX_print(gfx_p, buffer, 5, 13);
        }
        if(app_p->movements % 2 == 0){
            app_p->energy--;
            snprintf(buffer, BUFFER_SIZE, "%d", app_p->energy);
            GFX_print(gfx_p, buffer, 3, 11);
        }
    }
}

void Tamagotchi_movingLeft(TamagotchiApp* app_p, GFX* gfx_p, Joystick *joystick_p){
    char buffer[BUFFER_SIZE];
    if(Joystick_isTappedLeft(joystick_p) && app_p->spot > -3 && app_p->energy > 0){
        app_p->spot--;
        app_p->spotloc -= 10;
        app_p->needRemoved = true;
        app_p->movements++;
        if(app_p->happiness < 5){
            app_p->happiness++;
            snprintf(buffer, BUFFER_SIZE, "%d", app_p->happiness);
            GFX_print(gfx_p, buffer, 5, 13);
        }
        if(app_p->movements % 2 == 0){
            app_p->energy--;
            snprintf(buffer, BUFFER_SIZE, "%d", app_p->energy);
            GFX_print(gfx_p, buffer, 3, 11);
        }
    }
}

void Tamagotchi_childState(TamagotchiApp* app_p, GFX* gfx_p){
    if(app_p->needRemoved){
        GFX_removeSolidCircle(gfx_p, app_p->spotloc + 10, 85, 8);
        GFX_removeSolidCircle(gfx_p, app_p->spotloc - 10, 85, 8);
        app_p->needRemoved = false;
    }
    Graphics_setForegroundColor(&gfx_p->context, GRAPHICS_COLOR_GREEN);
    Graphics_fillCircle(&gfx_p->context, app_p->spotloc, 85, 8);
    Graphics_setForegroundColor(&gfx_p->context, GRAPHICS_COLOR_BLACK);
    if(app_p->age >= 3 && app_p->energy >= 3 && app_p->happiness >= 4){
        app_p->waitToPass = app_p->age + 1;
        app_p->gamespot = TEEN;
    }
}

void Tamagotchi_teenState(TamagotchiApp* app_p, GFX* gfx_p){
    if(app_p->needRemoved){
        GFX_removeSolidCircle(gfx_p, app_p->spotloc + 10, 85, 10);
        GFX_removeSolidCircle(gfx_p, app_p->spotloc - 10, 85, 10);
        app_p->needRemoved = false;
    }
    Graphics_setForegroundColor(&gfx_p->context, GRAPHICS_COLOR_BLUE);
    Graphics_fillCircle(&gfx_p->context, app_p->spotloc, 85, 10);
    Graphics_setForegroundColor(&gfx_p->context, GRAPHICS_COLOR_BLACK);
    if(app_p->age >= 7 && app_p->energy >= 2 && app_p->happiness >= 2 && app_p->age > app_p->waitToPass){
        app_p->gamespot = ADULT;
        app_p->waitToPass = 0;
    }
}

void Tamagotchi_adultState(TamagotchiApp* app_p, GFX* gfx_p){
    if(app_p->needRemoved){
        GFX_removeSolidCircle(gfx_p, app_p->spotloc + 10, 85, 12);
        GFX_removeSolidCircle(gfx_p, app_p->spotloc - 10, 85, 12);
        app_p->needRemoved = false;
    }
    Graphics_setForegroundColor(&gfx_p->context, GRAPHICS_COLOR_RED);
    Graphics_fillCircle(&gfx_p->context, app_p->spotloc, 85, 12);
    Graphics_setForegroundColor(&gfx_p->context, GRAPHICS_COLOR_BLACK);
}
