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
#include "screenshot.h"
#include "textures.h"
#include "log.h"
#include "kernel_functions.h"
#include "utils.h"
#include "btController.h"
#include "btAnimations.h"
#include "game.h"

namespace GUI {
    static MenuItem item;
    static uint8_t highlightedDemo;
    static uint8_t newConnectionsEnabled = 0;
    static uint8_t controllersPaired = 0;
    static int playerLEDValue = 0;

    void ResetCheckbox(MenuItem &item) {
        item.checked.clear();
        item.checked_copy.clear();
        item.checked.resize(item.entries.size());
        item.checked.assign(item.checked.size(), false);
        item.checked_cwd.clear();
        item.checked_count = 0;
    };

    void GetStorageSize(MenuItem &item) {
        item.total_storage = Utils::GetTotalStorage();
        item.used_storage = Utils::GetUsedStorage();
    }

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

    void DemoSelect() {
        BTController::LoadControllerState();

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
                    item.state = MAIN_STATE_GAME;
                    highlightedDemo = 0;
                    /* code */
                    break;
                case 1:
                    item.state = MAIN_STATE_VIBRATION_DEMO;
                    highlightedDemo = 0;
                    /* code */
                    break;
                case 2:
                    item.state = MAIN_STATE_COLOUR_DEMO;
                    highlightedDemo = 0;
                    /* code */
                    break;
                case 3:
                    item.state = MAIN_STATE_PLAYERCOUNT_DEMO;
                    highlightedDemo = 0;
                    /* code */
                    break;
            
            }
        }

    }

    void DisplayColourDemo() {
        BTController::LoadControllerState();
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

        // print analog values to screen
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

        if(BTController::IsMiscHeld(0, BT_MISC_BUTTON_START)){
            item.state = MAIN_STATE_DEMO_SELECT;
        }

        BTController::SetColour(0, leftAnalogX, leftAnalogY, rightAnalogX);
    }

    void DisplayPlayerCountDemo() {
        BTController::LoadControllerState();
        G2D::FontSetStyle(2.f, TITLE_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 40, "Player Count Demo");

        G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_CENTER);  
        G2D::DrawText(480 / 2, 100, "It is possible to control the player count LED\n"
                                    "Press X to cycle the numbers.");

        if (BTController::IsButtonPressed(0, BT_BUTTON_X)) {
            playerLEDValue++;
            BTController::setPlayerLED(0, playerLEDValue & 0x0f);
        }

        if(BTController::IsMiscHeld(0, BT_MISC_BUTTON_START)){
            item.state = MAIN_STATE_DEMO_SELECT;
        }
    }

    void DisplayVibratonDemo() {
        BTController::LoadControllerState();
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
            item.state = MAIN_STATE_DEMO_SELECT;
        }

        BTController::SetVibration(0, leftAnalogY, rightAnalogY, 0, 1000);
    }

    void DisplayMainIntro() {
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
            if( Utils::IsButtonPressed(PSP_CTRL_START)) {
                item.state = MAIN_STATE_CONROLER_SETUP;
            }
        }
    }

    void DisplayControlerSetup() {
        ControllerInfo ctrlOne;
        ControllerInfo ctrlTwo;

        if (!newConnectionsEnabled) {
            BTController::enablePairing();
            newConnectionsEnabled = 1;
        }

        BTController::LoadControllerState();

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

                RenderImage(ctrlOne.controllerModel, columnOneCenter, 40, battery_val, percent);

                G2D::DrawText(columnOneCenter, 160, "Controller One Connected");
            }

            if (!ctrlTwo.connected)
                G2D::DrawText(columnTwoCenter, 160, "Please Connect Controller Two");
            else { 
                int percent = (static_cast<float>(ctrlTwo.batteryLevel) / 255) * 100;
                int battery_val = (percent / 20);

                RenderImage(ctrlTwo.controllerModel, columnTwoCenter, 40, battery_val, percent);

                G2D::DrawText(columnTwoCenter, 160, "Controller Two Connected");
            }

            if (ctrlOne.connected && ctrlTwo.connected) {
                G2D::FontSetStyle(1.f, GREEN, INTRAFONT_ALIGN_CENTER);  

                int confirm_width = intraFontMeasureText(font, "Press START to continue.");
        
                G2D::DrawRect((480 / 2) - (confirm_width / 2) - 5, (180 - (font->texYSize - 15)) - 5, confirm_width + 10, (font->texYSize - 5) + 10, SELECTOR_COLOUR);
                G2D::DrawText(480 / 2, (192 - (font->texYSize - 15)) - 3, "Press START to continue.");

                if(BTController::IsMiscHeld(0, BT_MISC_BUTTON_START)) {
               // if(BTController::IsMiscPressed(0, BT_MISC_BUTTON_START) || BTController::IsMiscPressed(1, BT_MISC_BUTTON_START)) {
                    item.state = MAIN_STATE_DEMO_SELECT;
                    BTController::disablePairing();
                    newConnectionsEnabled = 0;
                }
            }
        } else {
            G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_LEFT);
            G2D::DrawText(10, 250, "Error Getting Data");
        }
    }

    void DisplayStatusBar(void) {
        ScePspDateTime time;
        static char time_string[30];

        if (R_SUCCEEDED(sceRtcGetCurrentClockLocalTime(&time)))
            std::snprintf(time_string, 30, "%2i:%02i %s", ((time.hour % 12) == 0)? 12 : time.hour % 12, time.minute, (time.hour / 12)? "PM" : "AM");

        G2D::FontSetStyle(1.f, WHITE, INTRAFONT_ALIGN_LEFT);
        G2D::DrawText(5, 14, time_string);

        int state = 0, percent = 0, battery_val = 0;
        if (scePowerIsBatteryExist()) {
            state = scePowerIsBatteryCharging();
            percent = scePowerGetBatteryLifePercent();
            battery_val = (percent / 20);
        }
        
        static char percent_string[10];
        std::snprintf(percent_string, 10, "%d", percent);
        int percent_width = intraFontMeasureText(font, percent_string);
        intraFontPrintf(font, 475 - percent_width - 15, 14, "%s%%", percent_string);

        G2D::DrawImage(state != 0? battery_charging[battery_val] : battery[battery_val], 475 - percent_width - battery[battery_val]->w - 15, 2);
    }

    void ProgressBar(const std::string &title, std::string message, u64 offset, u64 size) {
        if (message.length() > 35) {
            message.resize(35);
            message.append("...");
        }
        
        g2dClear(BG_COLOUR);
        G2D::DrawRect(0, 0, 480, 18, STATUS_BAR_COLOUR);
        G2D::DrawRect(0, 18, 480, 34, MENU_BAR_COLOUR);
        G2D::DrawImageScale(icon_nav_drawer, 5, 24, 26.f, 26.f);
        GUI::DisplayStatusBar();
        GUI::DisplayFileBrowser(item);
        
        G2D::DrawRect(0, 18, 480, 254, G2D_RGBA(0, 0, 0, cfg.dark_theme? 50 : 80));
        G2D::DrawImage(dialog[cfg.dark_theme], ((480 - (dialog[0]->w)) / 2), ((272 - (dialog[0]->h)) / 2));
        G2D::FontSetStyle(1.f, TITLE_COLOUR, INTRAFONT_ALIGN_LEFT);
        G2D::DrawText(((480 - (dialog[0]->w)) / 2) + 10, ((272 - (dialog[0]->h)) / 2) + 20, title.c_str());

        int text_width = intraFontMeasureText(font, message.c_str());
        G2D::FontSetStyle(1.f, TEXT_COLOUR, INTRAFONT_ALIGN_LEFT);
        G2D::DrawText(((480 - (text_width)) / 2), ((272 - (dialog[0]->h)) / 2) + 60, message.c_str());
        
        G2D::DrawRect(((480 - dialog[0]->w) / 2) + 20, ((272 - dialog[0]->h) / 2) + 70, 318, 4, SELECTOR_COLOUR);
        G2D::DrawRect(((480 - dialog[0]->w) / 2) + 20, ((272 - dialog[0]->h) / 2) + 70,
            static_cast<int>((static_cast<float>(offset) / static_cast<float>(size)) * 318.f), 4, TITLE_COLOUR);
        
        g2dFlip(G2D_VSYNC);
    }

    int RenderLoop(void) {
        int ret = 0;
        if (R_FAILED(ret = FS::GetDirList(cfg.cwd, item.entries)))
           return ret;

        GUI::ResetCheckbox(item);
        GUI::GetStorageSize(item);

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
            
            int ctrl = Utils::ReadControls();

            g2dClear(BG_COLOUR);

            //G2D::FontSetStyle(1.f, BLACK, INTRAFONT_ALIGN_LEFT);

            switch(item.state) {
                case MAIN_STATE_GAME:
                    Game::MainLoop(delta);

                    G2D::FontSetStyle(1.f, GREEN, INTRAFONT_ALIGN_CENTER);  
                    G2D::DrawText(480 / 2, 250, "Press START to return to menu.");

                    if(BTController::IsMiscHeld(0, BT_MISC_BUTTON_START)) {
                        item.state = MAIN_STATE_DEMO_SELECT;
                    }
                    break;
                case MAIN_STATE_INTRO:
                    GUI::DisplayMainIntro();
                    break;
                case MAIN_STATE_CONROLER_SETUP:
                    GUI::DisplayControlerSetup();
                    break;
                case MAIN_STATE_VIBRATION_DEMO:
                    GUI::DisplayVibratonDemo();
                    break;
                case MAIN_STATE_COLOUR_DEMO:
                    GUI::DisplayColourDemo();
                    break;
                case MAIN_STATE_PLAYERCOUNT_DEMO:
                    GUI::DisplayPlayerCountDemo();
                    break;
                case MAIN_STATE_DEMO_SELECT:
                    GUI::DemoSelect();
                    break;
            }
                    

            //static char percent_string[10];
           // std::snprintf(percent_string, 10, "%d", pspUARTAvailable());
            //G2D::DrawText(50, 90 + ((30 - (font->glyph->height - 6)) / 2), percent_string);

            g2dFlip(G2D_VSYNC);

            if (Utils::IsButtonPressed(static_cast<enum PspCtrlButtons>(PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER)))
                Screenshot::Capture();
        }

        return 0;
    }
}
