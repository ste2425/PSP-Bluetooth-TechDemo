#include "gui.h"

#include "btController.h"
#include "g2d.h"
#include "colours.h"

namespace GUI
{
    static int playerLEDValue = 0;
    static int playerLEDValue2 = 0;
    
    void PlayerIndicatorDemo(MainState& mainState) {
       // BTController::LoadControllerState();
        G2D::FontSetStyle(2.f, TITLE_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 40, "Player Count Demo");

        G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 100, "It is possible to control the player count LED\n"
                                    "Press X to cycle the numbers.");

        if (BTController::IsButtonPressed(0, BT_BUTTON_X)) {
            playerLEDValue++;
            BTController::setPlayerLED(0, playerLEDValue & 0x0f);
        }

        if (BTController::IsButtonPressed(1, BT_BUTTON_X)) {
            playerLEDValue2++;
            BTController::setPlayerLED(1, playerLEDValue2 & 0x0f);
        }

        if(BTController::IsMiscHeld(0, BT_MISC_BUTTON_START)){
            mainState.state = MAIN_STATE_DEMO_SELECT;
        }
    }
}