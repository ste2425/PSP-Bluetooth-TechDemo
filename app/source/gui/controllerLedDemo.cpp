#include "gui.h"

#include "btController.h"
#include "g2d.h"
#include "colours.h"

namespace GUI
{
    void ControllerLEDDemo(MainState& mainState) {
        G2D::FontSetStyle(2.f, TITLE_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 40, "Colour Demo");

        G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 100, "Some controllers have built in LED's, it is possible to control them.\n"
                                    "Use the analog sticks to control the RGB values.\n\n"
                                    "Left X axis controlls the Red value\n"
                                    "Left Y axs controlls the Green value\n"
                                    "Right X axis controlls the Blue value.");

        auto leftAnalogY = BTController::GetAnalogLY(0);
        auto leftAnalogX = BTController::GetAnalogLX(0);
        auto rightAnalogX = BTController::GetAnalogRY(0);

        G2D::FontSetStyle(1.f, TITLE_COLOUR, INTRAFONT_ALIGN_CENTER); 

        static char RValue[60];
        std::snprintf(RValue, 30, "Red: %d", leftAnalogX);
        G2D::DrawText(480 / 2, 180, RValue);

        static char gValue[60];
        std::snprintf(gValue, 30, "Green: %d", leftAnalogY);
        G2D::DrawText(480 / 2, 195, gValue);

        static char bValue[60];
        std::snprintf(bValue, 30, "Blue: %d", rightAnalogX);
        G2D::DrawText(480 / 2, 210, bValue);

        G2D::FontSetStyle(1.f, GREEN, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 250, "Press START to return to menu.");

        if(BTController::IsButtonPressed(0, BT_BUTTON_X) || BTController::IsMiscPressed(0, BT_MISC_BUTTON_START) || BTController::IsMiscPressed(1, BT_MISC_BUTTON_START)) {
            mainState.state = MAIN_STATE_DEMO_SELECT;
        }

        BTController::SetColour(0, leftAnalogX, leftAnalogY, rightAnalogX);
    }
}
