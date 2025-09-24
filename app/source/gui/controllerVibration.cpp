#include "gui.h"

#include "btController.h"
#include "g2d.h"
#include "colours.h"

namespace GUI {
    void ControllerVibrationDemo(MainState& mainState) {
        //BTController::LoadControllerState();
        G2D::FontSetStyle(2.f, TITLE_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 40, "Vibration Demo");

        G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 100, "It is possible to control the magnitude of a strong and weak motor\n"
                                    "Not all controllers have dual motors however, in this situation the\n"
                                    "strong moor value will be used.\n\n"
                                    "Use the analog sticks of player one to controll both these motors.");

        auto leftAnalogY = BTController::GetAnalogLY(0);
        auto rightAnalogY = BTController::GetAnalogRY(0);

        G2D::FontSetStyle(1.f, TITLE_COLOUR, INTRAFONT_ALIGN_CENTER); 

        // print analog values to screen
        static char leftValue[60];
        std::snprintf(leftValue, 30, "Weak Magnitude: %d", leftAnalogY);
        G2D::DrawText(480 / 2, 180, leftValue);

        static char rightValue[60];
        std::snprintf(rightValue, 30, "Strong Magnitude: %d", rightAnalogY);
        G2D::DrawText(480 / 2, 195, rightValue);

        G2D::FontSetStyle(1.f, GREEN, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 250, "Press START to return to menu.");

        if(BTController::IsMiscHeld(0, BT_MISC_BUTTON_START)) {
            mainState.state = MAIN_STATE_DEMO_SELECT;
        }

        BTController::SetVibration(0, leftAnalogY, rightAnalogY, 0, 1000);
    }
}