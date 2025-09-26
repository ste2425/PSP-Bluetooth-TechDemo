#include "gui.h"

#include "btController.h"
#include "colours.h"
#include "g2d.h"
#include "textures.h"
#include "config.h"

namespace GUI {
    static uint8_t newConnectionsEnabled = 0;

    void RenderImage(uint8_t controllerType, float x, float y, int battery_val, int percent) {
        switch(controllerType) {
            case CONTROLLER_TYPE_PS5Controller:
                G2D::DrawImageScale(dualsense_icon[cfg.dark_theme], x - 32, y, 64, 42);
                break;
            case CONTROLLER_TYPE_SwitchProController:
            case CONTROLLER_TYPE_SwitchJoyConLeft:
            case CONTROLLER_TYPE_SwitchJoyConRight:
            case CONTROLLER_TYPE_SwitchJoyConPair:
            case CONTROLLER_TYPE_SwitchInputOnlyController:
                G2D::DrawImageScale(switch_icon[cfg.dark_theme], x - 50, y, 100, 42);
                break;
            default:
                break;
        }

        static char percent_string[10];
        std::snprintf(percent_string, 10, "%d", percent);
        int percent_width = intraFontMeasureText(font, percent_string);
        intraFontPrintf(font, x, y + 60, "%s%%", percent_string);

        G2D::DrawImage(battery[battery_val], x - percent_width - battery[battery_val]->w, y + 48);
    }

    void DisplayControllerSetup(MainState& mainState) {
        ControllerInfo ctrlOne;
        ControllerInfo ctrlTwo;

        if (!newConnectionsEnabled) {
            BTController::enablePairing();
            newConnectionsEnabled = 1;
        }

        if (BTController::loadControllerInfo(0, ctrlOne) &&
            BTController::loadControllerInfo(1, ctrlTwo)) {
            G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_CENTER);  
            auto screenCenter = 480 / 2;
            auto columnOneCenter = screenCenter / 2;
            auto columnTwoCenter = screenCenter + columnOneCenter;

            // draw line in middle of screen vertically
            G2D::DrawRect(screenCenter - 1, 0, 2, 272, TITLE_COLOUR);

            if (!ctrlOne.connected)
                G2D::DrawText(columnOneCenter, 160, "Please Connect Controller One");
            else {
                int percent = (static_cast<float>(ctrlOne.batteryLevel) / 255) * 100;
                int battery_val = (percent / 20);

                GUI::RenderImage(ctrlOne.controllerModel, columnOneCenter, 40, battery_val, percent);

                G2D::DrawText(columnOneCenter, 160, "Controller One Connected");
            }

            if (!ctrlTwo.connected)
                G2D::DrawText(columnTwoCenter, 160, "Please Connect Controller Two");
            else { 
                int percent = (static_cast<float>(ctrlTwo.batteryLevel) / 255) * 100;
                int battery_val = (percent / 20);

                GUI::RenderImage(ctrlTwo.controllerModel, columnTwoCenter, 40, battery_val, percent);

                G2D::DrawText(columnTwoCenter, 160, "Controller Two Connected");
            }

            if (ctrlOne.connected && ctrlTwo.connected) {
                G2D::FontSetStyle(1.f, GREEN, INTRAFONT_ALIGN_CENTER);  

                int confirm_width = intraFontMeasureText(font, "Press START to continue.");
        
                G2D::DrawRect((480 / 2) - (confirm_width / 2) - 5, (180 - (font->texYSize - 15)) - 5, confirm_width + 10, (font->texYSize - 5) + 10, SELECTOR_COLOUR);
                G2D::DrawText(480 / 2, (192 - (font->texYSize - 15)) - 3, "Press START to continue.");

                if(BTController::IsMiscPressed(0, BT_MISC_BUTTON_START) || BTController::IsMiscPressed(1, BT_MISC_BUTTON_START)) {
                    mainState.state = MAIN_STATE_DEMO_SELECT;
                    BTController::disablePairing();
                    newConnectionsEnabled = 0;
                }
            }
        } else {
            G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_LEFT);
            G2D::DrawText(10, 250, "Error Getting Data");
        }
    }
}