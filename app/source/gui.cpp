#include <psppower.h>
#include <psprtc.h>

#include <pspctrl.h>
#include <pspkernel.h>
#include <pspumd.h>
#include <pspsysevent.h>

#include "colours.h"
#include "config.h"
#include "fs.h"
#include "g2d.h"
#include "gui.h"
#include "textures.h"
#include "log.h"
#include "kernel_functions.h"
#include "utils.h"
#include "btController.h"
#include "btAnimations.h"

namespace GUI {
    static MainState mainState;

    int RenderLoop(void) {
        Colours::Get();

        // Delta time
        u64 last_tick = 0;
        float resolution = sceRtcGetTickResolution() / 1000.f;
        sceRtcGetCurrentTick(&last_tick);

        while(running) {          
            u64 current_tick = 0;
            sceRtcGetCurrentTick(&current_tick);
            float delta = (current_tick - last_tick) / resolution;
            last_tick = current_tick;

            if (delta < 0.001f)
                delta = 0.001f;
            
            Utils::ReadControls();
            BTController::loadControllerStateChange(0);
            BTController::loadControllerStateChange(1);

            g2dClear(BG_COLOUR);

            switch(mainState.state) {
                case MAIN_STATE_GAME:
                    GUI::MainGameLoop(delta, mainState);
                    break;
                case MAIN_STATE_INTRO:
                    GUI::DisplayMainIntro(mainState);
                    break;
                case MAIN_STATE_CONROLER_SETUP:
                    GUI::DisplayControllerSetup(mainState);
                    break;
                case MAIN_STATE_VIBRATION_DEMO:
                    GUI::ControllerVibrationDemo(mainState);
                    break;
                case MAIN_STATE_COLOUR_DEMO:
                    GUI::ControllerLEDDemo(mainState);
                    break;
                case MAIN_STATE_PLAYERCOUNT_DEMO:
                    GUI::PlayerIndicatorDemo(mainState);
                    break;
                case MAIN_STATE_DEMO_SELECT:
                    GUI::DemoSelect(mainState);
                    break;
            }

            g2dFlip(G2D_VSYNC);
        }

        return 0;
    }
}
