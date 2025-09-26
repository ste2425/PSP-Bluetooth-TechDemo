#include "gui.h"

#include "btController.h"
#include "g2d.h"
#include "colours.h"

namespace GUI {
    static uint8_t highlightedDemo = 0;

    void DemoSelect(MainState& mainState) {
        G2D::FontSetStyle(2.f, TITLE_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 40, "Demo Select");
        int demoWidth = intraFontMeasureText(font, "Demo Select");

        if (BTController::IsDpadPressed(0, BT_DPAD_DOWN) && highlightedDemo < 3) {
            highlightedDemo++;
        } else if (BTController::IsDpadPressed(0, BT_DPAD_UP) && highlightedDemo > 0) {
            highlightedDemo--;
        }

        G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_CENTER);  

        if (highlightedDemo == 0)
            G2D::DrawRect((480 / 2) - (demoWidth / 2) - 5, (88 - (font->texYSize - 15)) - 5, demoWidth + 10, (font->texYSize - 5) + 10, SELECTOR_COLOUR);
        
        G2D::DrawText(480 / 2, (100 - (font->texYSize - 15)) - 3, "Pong");

        if (highlightedDemo == 1)
            G2D::DrawRect((480 / 2) - (demoWidth / 2) - 5, (108 - (font->texYSize - 15)) - 5, demoWidth + 10, (font->texYSize - 5) + 10, SELECTOR_COLOUR);
        
        G2D::DrawText(480 / 2, (120 - (font->texYSize - 15)) - 3, "Vibration");

        if (highlightedDemo == 2)
            G2D::DrawRect((480 / 2) - (demoWidth / 2) - 5, (128 - (font->texYSize - 15)) - 5, demoWidth + 10, (font->texYSize - 5) + 10, SELECTOR_COLOUR);
        
        G2D::DrawText(480 / 2, (140 - (font->texYSize - 15)) - 3, "Colour");

        if (highlightedDemo == 3)
            G2D::DrawRect((480 / 2) - (demoWidth / 2) - 5, (148 - (font->texYSize - 15)) - 5, demoWidth + 10, (font->texYSize - 5) + 10, SELECTOR_COLOUR);
        
        G2D::DrawText(480 / 2, (160 - (font->texYSize - 15)) - 3, "Player No.");

        if (BTController::IsButtonPressed(0, BT_BUTTON_X)) {
            switch (highlightedDemo)
            {
                case 0:
                    mainState.state = MAIN_STATE_GAME;
                    highlightedDemo = 0;
                    break;
                case 1:
                    mainState.state = MAIN_STATE_VIBRATION_DEMO;
                    highlightedDemo = 0;
                    break;
                case 2:
                    mainState.state = MAIN_STATE_COLOUR_DEMO;
                    highlightedDemo = 0;
                    break;
                case 3:
                    mainState.state = MAIN_STATE_PLAYERCOUNT_DEMO;
                    highlightedDemo = 0;
                    break;            
            }
        }
    }
}