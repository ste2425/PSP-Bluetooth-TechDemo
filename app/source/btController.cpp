#include "btController.h"
#include "log.h"
#include "g2d.h"
#include <pspkernel.h>
#include <psprtc.h>
#include "mutex.h"

namespace BTController
{

    static SceUID command_mtx = 0;

    static BTCtr liveControllers[2];
    static BTCtr controllers[2];
    static BTCtr previousControllers[2];   

    void init() {
        command_mtx = sceKernelCreateMutex("serial_command_mutex", PSP_MUTEX_ATTR_FIFO, 0, nullptr);
    }

    void deinit() {
        sceKernelDeleteMutex(command_mtx);
    }

    bool isKnownError(uint8_t status, uint8_t* errorCodes, uint8_t errorCodesSize) {
        if (errorCodes == nullptr || errorCodesSize == 0) {
            return false;
        }
        for (uint8_t i = 0; i < errorCodesSize; ++i) {
            if (status == errorCodes[i]) {
                return true;
            }
        }
        return false;
    }

    uint8_t sendCommand(
        uint8_t command, 
        uint8_t* commandArguments, 
        uint8_t commandArgumentsSize, 
        uint8_t successResponseCode, 
        uint8_t* knownErrorResponseCodes,
        uint8_t knownErrorResponseCodeSize,
        int* responseBuffer, 
        int responseSize
    ) {
        sceKernelLockMutex(command_mtx, 1, nullptr);

        // TODO figure out why this delay is needed
        sceKernelDelayThread(6000);

        // send command and any command arguments
        pspUARTWrite(command);

        if (commandArguments != nullptr && commandArgumentsSize != 0) {
            for (size_t i = 0; i < commandArgumentsSize; ++i) {
                pspUARTWrite(commandArguments[i]);
            }
        }

        sceKernelDelayThread(6000);
        int tries = 0;

        // wait for success response code
        int status = pspUARTRead();

        while (status != successResponseCode &&
            !isKnownError(status, knownErrorResponseCodes, knownErrorResponseCodeSize) &&
               tries < 20) 
        {
            sceKernelDelayThread(6000);
            status = pspUARTRead();
            tries++;
        }

        if (status != successResponseCode) {
            pspUARTResetRingBuffer();

            sceKernelUnlockMutex(command_mtx, 1);
            return status;
        }

        // This comand has no extra response data
        if (responseBuffer == nullptr || responseSize == 0) {
            sceKernelUnlockMutex(command_mtx, 1);
            return status;
        }

        // wait for enough data to have been recieved
        int recievedDataCount = pspUARTAvailable();
        tries = 0;

        while (recievedDataCount < responseSize && tries < 20) 
        {
            sceKernelDelayThread(20000);
            //pspUARTWaitForData(20000);
            recievedDataCount = pspUARTAvailable();
            tries++;
        }

        if (tries >= 20 && recievedDataCount < responseSize) {
            pspUARTResetRingBuffer();
            sceKernelUnlockMutex(command_mtx, 1);
            return RESPONSE_TIMEOUT;
        }

        // Read recieved data from SIO buffer
        for (int i = 0; i < responseSize; ++i) {
            responseBuffer[i] = pspUARTRead();
        }

        sceKernelUnlockMutex(command_mtx, 1);
        return status;
    }

    void setPlayerLED(uint8_t controllerIndex, uint8_t ledValue) {      
        uint8_t commandArgs[2] = {controllerIndex, ledValue};

        uint8_t response = sendCommand(
            COMMAND_SETPLAYERLED,
            commandArgs, 2,
            RESPONSE_PLAYERLED_OK,
            (uint8_t[]){RESPONSE_CONTROLLER_NOT_FOUND, RESPONSE_DATA_NOT_RECIEVED}, 2,
            nullptr, 0
        );

        if (response != RESPONSE_PLAYERLED_OK && response != RESPONSE_CONTROLLER_NOT_FOUND) {
            Log::Error("Failed to set player LED: %d\n", response);
        }
    }

    bool loadControllerInfo(uint8_t controllerIndex, ControllerInfo &info) {
        int responseBuffer[2] = {0};
        uint8_t responseBufferSize = 2;
        uint8_t commandArgs[1] = {controllerIndex};

        uint8_t response = sendCommand(
            COMMAND_GETCONTROLLERINFO,
            commandArgs, 1,
            RESPONSE_INFO_OK,
            (uint8_t[]){RESPONSE_CONTROLLER_NOT_FOUND, RESPONSE_DATA_NOT_RECIEVED}, 2,
            responseBuffer, responseBufferSize
        );

        if (response == RESPONSE_INFO_OK) {
            info.connected = true;
            info.controllerModel = responseBuffer[0];
            info.batteryLevel = responseBuffer[1];
            
            return true;
        } else if (response == RESPONSE_CONTROLLER_NOT_FOUND) {
            info.connected = false;
            info.batteryLevel = 0;
            info.controllerModel = CONTROLLER_TYPE_None;
            return true;
        } else {
            static char errorString[40];
            std::snprintf(errorString, 40, "Failed to get controller info: %d\n", response);
            Log::Error(errorString);
            return false;
        }
    }

    void loadControllerStateChange(uint8_t controllerIndex) {
        previousControllers[controllerIndex] = controllers[controllerIndex];
        controllers[controllerIndex] = liveControllers[controllerIndex];
    }
    
    void loadControllerData(uint8_t controllerIndex) {    

        int responseBuffer[9] = {0};
        uint8_t responseBufferSize = 9;
        uint8_t commandArgs[1] = {controllerIndex};

        uint8_t response = sendCommand(
            COMMAND_LOADCONTROLLERDATA,
            commandArgs, 1,
            RESPONSE_CONTROLLERDATA_OK,
            (uint8_t[]){RESPONSE_CONTROLLER_NOT_FOUND, RESPONSE_DATA_NOT_RECIEVED}, 2,
            responseBuffer, responseBufferSize
        );

        if (response == RESPONSE_CONTROLLERDATA_OK) {
            liveControllers[controllerIndex].connected = true;
            liveControllers[controllerIndex].index = controllerIndex;
            liveControllers[controllerIndex].analogRX = responseBuffer[0];
            liveControllers[controllerIndex].analogRY = responseBuffer[1];
            liveControllers[controllerIndex].analogLX = responseBuffer[2];
            liveControllers[controllerIndex].analogLY = responseBuffer[3];
            liveControllers[controllerIndex].dpad = responseBuffer[4];
            liveControllers[controllerIndex].buttons = responseBuffer[5] << 8;
            liveControllers[controllerIndex].buttons |= responseBuffer[6];
            liveControllers[controllerIndex].miscButtons = responseBuffer[7] << 8;
            liveControllers[controllerIndex].miscButtons |= responseBuffer[8];

            return;
        }

        liveControllers[controllerIndex].connected = false;
    }   

    bool enablePairing() {
        uint8_t response = sendCommand(
            COMMAND_ENABLENEWCONNECTIONS,
            nullptr, 0,
            RESPONSE_NEWCON_OK,
            nullptr, 0,
            nullptr, 0
        );   

        return response == RESPONSE_NEWCON_OK;
    }

    bool disablePairing() {
        uint8_t response = sendCommand(
            COMMAND_DISABLENEWCONNECTIONS,
            nullptr, 0,
            RESPONSE_DISNEWCON_OK,
            nullptr, 0,
            nullptr, 0
        );   

        return response == RESPONSE_DISNEWCON_OK;
    }

    uint8_t ping() {
        uint8_t response = sendCommand(
            COMMAND_PING,
            nullptr, 0,
            RESPONSE_PING,
            (uint8_t[]){RESPONSE_CONTROLLER_NOT_FOUND, RESPONSE_DATA_NOT_RECIEVED}, 2,
            nullptr, 0
        ); 

        return response;
    }

    void SetVibration(uint8_t controllerIndex, uint8_t weakMagnitude, uint8_t strongMagnitude, uint16_t delayMs, uint16_t durationMs)
    {
        uint8_t responseBufferSize = 2;
        uint8_t commandArgs[7] = {
            controllerIndex,
            (delayMs >> 8) & 0xFF, //Delay high byte
            delayMs & 0xFF, //Delay low byte
            (durationMs >> 8) & 0xFF, //Duration high byte
            durationMs & 0xFF, //Duration low byte
            weakMagnitude,
            strongMagnitude
        };

        uint8_t response = sendCommand(
            COMMAND_SETVIBRATION,
            commandArgs, 7,
            RESPONSE_VIBRATE_OK,
            (uint8_t[]){RESPONSE_CONTROLLER_NOT_FOUND, RESPONSE_DATA_NOT_RECIEVED}, 2,
            nullptr, 0
        );   

        if (response == RESPONSE_VIBRATE_OK) {
            return;
        }
        if (response == RESPONSE_DATA_NOT_RECIEVED) {
            Log::Error("Vibration Set Failed, Not enough data\n");
        }
        if (response == RESPONSE_CONTROLLER_NOT_FOUND) {
            Log::Error("Vibration Set Failed, Missing controller\n");
        }
    }

    void SetColour(uint8_t controllerIndex, uint8_t red, uint8_t green, uint8_t blue)
    {
        if (controllerIndex >= 2) {
            return; // Invalid controller index
        }
        
        uint8_t commandArgs[4] = {
            controllerIndex,
            red,
            green,
            blue
        };

        uint8_t response = sendCommand(
            COMMAND_SETLEDCOLOUR,
            commandArgs, 4,
            RESPONSE_LED_OK,
            (uint8_t[]){RESPONSE_CONTROLLER_NOT_FOUND, RESPONSE_DATA_NOT_RECIEVED}, 2,
            nullptr, 0
        );

        if (response == RESPONSE_LED_OK) {
            return;
        }
        if (response == RESPONSE_DATA_NOT_RECIEVED) {
            Log::Error("LED Set Failed, Not enough data\n");
        }
        if (response == RESPONSE_CONTROLLER_NOT_FOUND) {
            Log::Error("LED Set Failed, Missing controller\n");
        }
    }

    void LoadControllerState()
    {        
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
}
