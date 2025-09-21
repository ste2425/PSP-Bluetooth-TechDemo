#include "btController.h"
#include "log.h"
#include "g2d.h"
#include <pspkernel.h>
#include <psprtc.h>

namespace BTController
{
    static BTCtr controllers[2];
    static BTCtr previousControllers[2];   

    static uint8_t pendingColourChangeData[4];
    static uint8_t colourChangePending = 0;

    static uint8_t vibrationPending = 0;
    static uint8_t vibrationController = 0;
    static uint16_t vibrationDelay = 0;
    static uint16_t vibrationDuration = 0;
    static uint8_t vibratioWeakMagnitude = 0;
    static uint8_t vibrationStrongMagnitude = 0;
    static uint8_t playerLEDPending = 0;
    static uint8_t playerLEDValue = 0;

    void setPlayerLED(uint8_t controllerIndex, uint8_t ledValue) {
        playerLEDPending = 1;
        playerLEDValue = ledValue;
    }

    void processPlayerLED() {
        if (playerLEDPending == 1) {    
            playerLEDPending = 0;        

            pspUARTWrite(0x0C); //command byte
            pspUARTWrite(0); //controller index byte
            pspUARTWrite(playerLEDValue); //delay high byte
           // Log::Error("Data in buffer before: %d\n", pspUARTAvailable());
            //pspUARTWaitForData(10000);
            sceKernelDelayThread(6000);
            int tries = 0;

        // Log::Error("Queried controller response: %d\n", recievedDataCount);
            // it's possible weve not got all the bytes yet. Keep waiting untill we do.
            int resp = pspUARTRead();
            while (resp != RESPONSE_PLAYERLED_OK &&
                resp != RESPONSE_DATA_NOT_RECIEVED &&
                resp != RESPONSE_CONTROLLER_NOT_FOUND &&
                tries < 40) 
            {
                // Log::Error("Witing for Vibration data, got: %d\n", resp);
                sceKernelDelayThread(6000);
                    resp = pspUARTRead();
                    //Log::Error("Witing for controller data, got: %d\n", recievedDataCount);
                    tries++;
            }

            if (resp == RESPONSE_PLAYERLED_OK) {
                return;
            }
            if (resp == RESPONSE_DATA_NOT_RECIEVED) {
                Log::Error("Player LED Set Failed, Not enough data\n");
            }
            if (resp == RESPONSE_CONTROLLER_NOT_FOUND) {
                Log::Error("Player LED Set Failed, Missing controller\n");
            }

            if (tries >= 40) {
              Log::Error("Too many tries\n");
            }

            pspUARTResetRingBuffer();
        }
    }

    bool loadControllerInfo(uint8_t controllerIndex, ControllerInfo &info) {

            pspUARTWrite(0x0A); //command byte
            pspUARTWrite(controllerIndex); //controller index byte
            
            sceKernelDelayThread(6000);
            int tries = 0;

        // Log::Error("Queried controller response: %d\n", recievedDataCount);
            // it's possible weve not got all the bytes yet. Keep waiting untill we do.
            int resp = pspUARTRead();
            while (resp != RESPONSE_INFO_OK &&
                resp != RESPONSE_DATA_NOT_RECIEVED &&
                resp != RESPONSE_CONTROLLER_NOT_FOUND &&
                tries < 40) 
            {
                Log::Error("Witing for Vibration data, got: %d\n", resp);
                sceKernelDelayThread(6000);
                    resp = pspUARTRead();
                    //Log::Error("Witing for controller data, got: %d\n", recievedDataCount);
                    tries++;
            }

            if (resp == RESPONSE_CONTROLLER_NOT_FOUND) {
                info.connected = false;
                info.batteryLevel = 0;
                info.controllerModel = CONTROLLER_TYPE_None;

                return true;
            } else if (resp == RESPONSE_INFO_OK) {
                info.connected = true;

                int recievedDataCount = pspUARTAvailable();
                tries = 0;

                // Log::Error("Queried controller response: %d\n", recievedDataCount);
                // it's possible weve not got all the bytes yet. Keep waiting untill we do.
                while (recievedDataCount < 2 && tries < 20) 
                {
                    sceKernelDelayThread(20000);
                    //pspUARTWaitForData(20000);
                    recievedDataCount = pspUARTAvailable();
                    //Log::Error("Witing for Controller data, got: %d, value: %d\n", recievedDataCount, pspUARTPeek(0));
                    tries++;
                }

                if (tries >= 20) {
                   Log::Error("Too many tries Info\n");
                    pspUARTResetRingBuffer();
                    return false;
                }
                
                info.controllerModel = pspUARTRead();
                info.batteryLevel = pspUARTRead();

                return true;
            } else if (resp == RESPONSE_DATA_NOT_RECIEVED) {
                Log::Error("Info Query Failed, Not enough data\n");
            }

            return false;
    }
    
    void loadControllerData(uint8_t controllerIndex) {     
        pspUARTResetRingBuffer();   
        previousControllers[controllerIndex] = controllers[controllerIndex];

        //Log::Error("Loading Controller For, %d, x: %d\n", controllerIndex, pspUARTPeek(0));
        pspUARTWrite(0x03);
        pspUARTWrite(controllerIndex);
        //Log::Error("Data Bro, %d\n", pspUARTPeek(0));

       // pspUARTWaitForData(20000);
        sceKernelDelayThread(20000);
        int recievedDataCount = pspUARTAvailable();
        int tries = 0;

       // Log::Error("Queried controller response: %d\n", recievedDataCount);
        // it's possible weve not got all the bytes yet. Keep waiting untill we do.
        while (recievedDataCount < 10 && tries < 20) 
        {
            sceKernelDelayThread(20000);
                //pspUARTWaitForData(20000);
                recievedDataCount = pspUARTAvailable();
                //Log::Error("Witing for Controller data, got: %d, value: %d\n", recievedDataCount, pspUARTPeek(0));
                tries++;
        }

        // TODO clear buffer if tries reaches max
        if (tries >= 20) {
           Log::Error("Too many tries Controller\n");
            pspUARTResetRingBuffer();
            return;
        }

        auto status = pspUARTRead();

        if (status == 0xA || status == 0xB) {
            controllers[controllerIndex].connected = false;
        }

        if (status == 0xC) {
            controllers[controllerIndex].connected = true;
            controllers[controllerIndex].index = controllerIndex;
            controllers[controllerIndex].analogRX = pspUARTRead();
            controllers[controllerIndex].analogRY = pspUARTRead();
            controllers[controllerIndex].analogLX = pspUARTRead();
            controllers[controllerIndex].analogLY = pspUARTRead();
            controllers[controllerIndex].dpad = pspUARTRead();
            controllers[controllerIndex].buttons = pspUARTRead() << 8;
            controllers[controllerIndex].buttons |= pspUARTRead();
            controllers[controllerIndex].miscButtons = pspUARTRead() << 8;
            controllers[controllerIndex].miscButtons |= pspUARTRead();

            /*Log::Error("Controller %d, LX: %d, LY: %d, RX: %d, RY: %d, DPad: %d, Buttons: 0x%04X, Misc: 0x%04X\n", 
                controllerIndex,
                controllers[controllerIndex].analogLX,
                controllers[controllerIndex].analogLY,
                controllers[controllerIndex].analogRX,
                controllers[controllerIndex].analogRY,
                controllers[controllerIndex].dpad,
                controllers[controllerIndex].buttons,
                controllers[controllerIndex].miscButtons);*/
        }
        //Log::Error("Processing done\n");

       // controllers[controllerIndex] = controllerData;
    }   

    void loadControllerVibration() {
        if (vibrationPending == 1) {    
            vibrationPending = 0;        

            pspUARTWrite(0x09); //command byte
            pspUARTWrite(vibrationController); //controller index byte
            pspUARTWrite((vibrationDelay >> 8) & 0xFF); //delay high byte
            pspUARTWrite(vibrationDelay & 0xFF); //delay low byte
            pspUARTWrite((vibrationDuration >> 8) & 0xFF); //duration high byte
            pspUARTWrite(vibrationDuration & 0xFF); //duration low byte
            pspUARTWrite(vibratioWeakMagnitude); //weak magnitude byte
            pspUARTWrite(vibrationStrongMagnitude); //strong magnitude byte
           // Log::Error("Data in buffer before: %d\n", pspUARTAvailable());
            //pspUARTWaitForData(10000);
            sceKernelDelayThread(6000);
            int tries = 0;

        // Log::Error("Queried controller response: %d\n", recievedDataCount);
            // it's possible weve not got all the bytes yet. Keep waiting untill we do.
            int resp = pspUARTRead();
            while (resp != RESPONSE_VIBRATE_OK &&
                resp != RESPONSE_DATA_NOT_RECIEVED &&
                resp != RESPONSE_CONTROLLER_NOT_FOUND &&
                tries < 40) 
            {
                // Log::Error("Witing for Vibration data, got: %d\n", resp);
                sceKernelDelayThread(6000);
                    resp = pspUARTRead();
                    //Log::Error("Witing for controller data, got: %d\n", recievedDataCount);
                    tries++;
            }

            if (resp == RESPONSE_VIBRATE_OK) {
                return;
            }
            if (resp == RESPONSE_DATA_NOT_RECIEVED) {
                Log::Error("Vibration Set Failed, Not enough data\n");
            }
            if (resp == RESPONSE_CONTROLLER_NOT_FOUND) {
                Log::Error("Vibration Set Failed, Missing controller\n");
            }

            if (tries >= 40) {
              Log::Error("Too many tries\n");
            }

            pspUARTResetRingBuffer();
        }
    }

    bool enablePairing() {
        pspUARTWrite(0x04); //command byte
        //pspUARTWaitForData(10000);
        sceKernelDelayThread(6000);
        int tries = 0;

    // Log::Error("Queried controller response: %d\n", recievedDataCount);
        // it's possible weve not got all the bytes yet. Keep waiting untill we do.
        int resp = pspUARTRead();
        while (resp != RESPONSE_NEWCON_OK &&
            tries < 40) 
        {
            // Log::Error("Witing for Vibration data, got: %d\n", resp);
            sceKernelDelayThread(6000);
                resp = pspUARTRead();
                //Log::Error("Witing for controller data, got: %d\n", recievedDataCount);
                tries++;
        }

        if (resp == RESPONSE_NEWCON_OK) {
            return true;
        }

        return false;
    }

    bool disablePairing() {
        pspUARTWrite(0x05); //command byte
        //pspUARTWaitForData(10000);
        sceKernelDelayThread(6000);
        int tries = 0;

    // Log::Error("Queried controller response: %d\n", recievedDataCount);
        // it's possible weve not got all the bytes yet. Keep waiting untill we do.
        int resp = pspUARTRead();
        while (resp != RESPONSE_DISNEWCON_OK &&
            tries < 40) 
        {
            // Log::Error("Witing for Vibration data, got: %d\n", resp);
            sceKernelDelayThread(6000);
                resp = pspUARTRead();
                //Log::Error("Witing for controller data, got: %d\n", recievedDataCount);
                tries++;
        }

        if (resp == RESPONSE_DISNEWCON_OK) {
            return true;
        }

        return false;
    }

    void loadControllerColour() {
        if (colourChangePending == 1) {    
            //Log::Error("Setting LED colour\n");
            colourChangePending = 0;        

            pspUARTWrite(0x07); //command byte
            pspUARTWrite(pendingColourChangeData[0]); //controller index byte
            pspUARTWrite(pendingColourChangeData[1]); //R byte
            pspUARTWrite(pendingColourChangeData[2]); //G byte
            pspUARTWrite(pendingColourChangeData[3]); //B byte

           // Log::Error("Data in buffer before: %d\n", pspUARTAvailable());
            //pspUARTWaitForData(10000);
            sceKernelDelayThread(6000);
            int tries = 0;

        // Log::Error("Queried controller response: %d\n", recievedDataCount);
            // it's possible weve not got all the bytes yet. Keep waiting untill we do.
            int resp = pspUARTRead();
            while (resp != 0xD &&
                resp != 0xA &&
                resp != 0xC &&
                tries < 40) 
            {
               // Log::Error("Witing for LED data, got: %d\n", resp);
            sceKernelDelayThread(6000);
                resp = pspUARTRead();
                //Log::Error("Witing for controller data, got: %d\n", recievedDataCount);
                tries++;
            }

            if (resp == 0xD) {
                return;
            }
            if (resp == 0xA) {
                Log::Error("LED Set Failed, Not enough data\n");
            }
            if (resp == 0xB) {
                Log::Error("LED Set Failed, Missing controller\n");
            }

            if (tries >= 40) {
              Log::Error("Too many tries\n");
            }

            pspUARTResetRingBuffer();
        }
    }

    void LoadControllerState()
    {
        loadControllerColour();
        loadControllerVibration();
        processPlayerLED();
        
        loadControllerData(0);
        loadControllerData(1);
    }

    int IsButtonPressed(uint8_t controllerIndex, uint16_t button)
    {
        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }
        //return controllers[controllerIndex].buttons;
        return (controllers[controllerIndex].buttons & button) && !(previousControllers[controllerIndex].buttons & button);
    }

    int IsButtonHeld(uint8_t controllerIndex, uint16_t button)
    {
        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }

        return controllers[controllerIndex].buttons & button;
    }

    int IsMiscPressed(uint8_t controllerIndex, uint16_t button)
    {
        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }

        return (controllers[controllerIndex].miscButtons & button) && !(previousControllers[controllerIndex].miscButtons & button);
    }

    int IsMiscHeld(uint8_t controllerIndex, uint16_t button)
    {
        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }

        return controllers[controllerIndex].miscButtons & button;
    }


    uint8_t GetAnalogRX(uint8_t controllerIndex) {

        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }
        auto pad = controllers[controllerIndex];

        return pad.analogRX - 122.5f;
    }

    uint8_t GetAnalogRY(uint8_t controllerIndex) {

        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }
        auto pad = controllers[controllerIndex];

        return pad.analogRY;
    }

    uint8_t GetAnalogLX(uint8_t controllerIndex) {

        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }
        auto pad = controllers[controllerIndex];

        return pad.analogLX;
    }

    uint8_t GetAnalogLY(uint8_t controllerIndex) {

        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }
        auto pad = controllers[controllerIndex];

        return pad.analogLY;
    }

    int IsDpadPressed(uint8_t controllerIndex, uint8_t dpadMask)
    {
        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }

        return (controllers[controllerIndex].dpad & dpadMask) && !(previousControllers[controllerIndex].dpad & dpadMask);
    }

    int IsDpadHeld(uint8_t controllerIndex, uint8_t dpadMask)
    {
        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }

        return controllers[controllerIndex].dpad & dpadMask;
    }

    int IsConnected(uint8_t controllerIndex)
    {
        if (controllerIndex >= 2) {
            return 0; // Invalid controller index
        }
        
        return controllers[controllerIndex].connected;
    }

    int IsPending() {
        return colourChangePending;
    }

    bool ping() {       
        pspUARTWrite(0x02); //command byte
        //pspUARTWaitForData(10000);
        sceKernelDelayThread(6000);
        int tries = 0;

    // Log::Error("Queried controller response: %d\n", recievedDataCount);
        // it's possible weve not got all the bytes yet. Keep waiting untill we do.
        int resp = pspUARTRead();
        while (resp != RESPONSE_PING &&
            tries < 40) 
        {
            // Log::Error("Witing for Vibration data, got: %d\n", resp);
            sceKernelDelayThread(6000);
                resp = pspUARTRead();
                //Log::Error("Witing for controller data, got: %d\n", recievedDataCount);
                tries++;
        }

        if (resp == RESPONSE_PING) {
            return true;
        }

        return false;
    }

    void SetVibration(uint8_t controllerIndex, uint8_t weakMagnitude, uint8_t strongMagnitude, uint16_t delayMs, uint16_t durationMs)
    {
        if (controllerIndex >= 2) {
            return; // Invalid controller index
        }

        vibrationPending = 1;
        vibrationController = controllerIndex;
        vibratioWeakMagnitude = weakMagnitude;
        vibrationStrongMagnitude = strongMagnitude;
        vibrationDelay = delayMs;
        vibrationDuration = durationMs;
    }

    void SetColour(uint8_t controllerIndex, uint8_t red, uint8_t green, uint8_t blue)
    {
        if (controllerIndex >= 2) {
            return; // Invalid controller index
        }

        colourChangePending = 1;
        pendingColourChangeData[0] = controllerIndex;
        pendingColourChangeData[1] = red;
        pendingColourChangeData[2] = green;
        pendingColourChangeData[3] = blue;
    }
}
