#include "gui.h"

#include "btController.h"
#include "colours.h"
#include "g2d.h"
#include "utils.h"

namespace GUI {
    void DisplayMainIntro(MainState& mainState) {
        G2D::FontSetStyle(2.f, TITLE_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 40, "PSP Bluetooth Test App");

        G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 100, "This application allows you to connect\n"
                                    "various Bluetooth controllers to your PSP.\n\n"
                                    "It communicates with an ESP32 connected via the AV .\n"
                                    "This demos some of the more advanced features including:\n"
                                    "* Dual controllers\n"
                                    "* Battery level detection\n"
                                    "* Controller type detection\n"
                                    "* Vibration + LED control");

        auto pinged = BTController::ping();
            
        if (!pinged) {
            G2D::FontSetStyle(1.f, RED, INTRAFONT_ALIGN_CENTER);  
            G2D::DrawText(480 / 2, 200, "Error: Could not communicate with ESP32.\n"
                                        "Please ensure it is connected to the PSP\n"
                                        "and powered on.");
        } else { 
            G2D::FontSetStyle(1.f, GREEN, INTRAFONT_ALIGN_CENTER);  
            G2D::DrawText(480 / 2, 200, "ESP32 Detected!\n"
                                        "Press START to continue.");
                                        
            if(Utils::IsButtonPressed(PSP_CTRL_START)) {
                mainState.state = MAIN_STATE_CONROLER_SETUP;
            }
        }
    }
}