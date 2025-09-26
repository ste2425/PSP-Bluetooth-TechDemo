#pragma once

#include <glib2d.h>
#include <pspiofilemgr.h>
#include <string>
#include <vector>

#define SCREEN_HEIGHT 272
#define SCREEN_WIDTH 480

enum MENU_STATES {
    MAIN_STATE_GAME,
    MAIN_STATE_INTRO,
    MAIN_STATE_CONROLER_SETUP,
    MAIN_STATE_VIBRATION_DEMO,
    MAIN_STATE_COLOUR_DEMO,
    MAIN_STATE_PLAYERCOUNT_DEMO,
    MAIN_STATE_DEMO_SELECT
};

typedef struct {
    MENU_STATES state = MAIN_STATE_INTRO;
} MainState;

extern bool running;

namespace GUI {
    int RenderLoop(void);

    void DisplayMainIntro(void);

    void MainGameLoop(float delta, MainState& mainState);

    void ControllerLEDDemo(MainState& mainState);

    void PlayerIndicatorDemo(MainState& mainState);

    void DisplayMainIntro(MainState& mainState);

    void DisplayControllerSetup(MainState& mainState);

    void ControllerVibrationDemo(MainState& mainState);

    void DemoSelect(MainState& mainState);
}
