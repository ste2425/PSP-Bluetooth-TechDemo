#include <pspctrl.h>
#include <pspkernel.h>
#include <pspumd.h>
#include <psppower.h>
#include <pspsysevent.h>

#include "config.h"
#include "g2d.h"
#include "gui.h"
#include "log.h"
#include "textures.h"
#include "utils.h"
#include "kernel_functions.h"
#include "colours.h"
#include "btController.h"

PSP_MODULE_INFO("CMFileManager", 0x800, VERSION_MAJOR, VERSION_MINOR);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_THRESHOLD_SIZE_KB(1024);
PSP_HEAP_SIZE_KB(-2048);

bool running = true;

#define MODULE_NAME "BTPONG"

    // Power Callback handler
    /*int power_callback_handler(int unknown, int pwrflags, void *common)
    {
        GUI::SetColour(TEXT_COLOUR_DARK);
        if (pwrflags & PSP_POWER_CB_POWER_SWITCH) {
            // This is called immediately as the switch is pressed.
            // The SysEventHandler is called when the power switch is released, or held down for a second.
            // This gives us a chance to get in before it and decide whether to allow the sleep.
            
            Log::Error("Power switch pressed\n");
        }
        else if (pwrflags & PSP_POWER_CB_RESUME_COMPLETE) {
            // The system has resumed from sleep
            Log::Error("System resumed from sleep\n");
            // Reinitialize resources if needed
        }
        else if (pwrflags & PSP_POWER_CB_SUSPENDING) {
            // The system is about to go to sleep
            Log::Error("System suspending\n");
            // Save state or release resources if needed
        }
        else if (pwrflags & PSP_POWER_CB_RESUMING) {
            // The system is resuming from sleep
            Log::Error("System resuming\n");
            // Prepare to reinitialize resources if needed
        }
        else {
            Log::Error("Shutdown from other reason maye?");
        }

        return 0;
    }
*/
namespace Services {
    int Init(void) {
        int ret = 0;

        sceCtrlSetSamplingCycle(0);
        sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
        Utils::InitKernelDrivers();
        Textures::Load();

        Utils::IsMemCardInserted(isMSInserted);
        isPSPGo = Utils::IsModelPSPGo();
        
        if (R_FAILED(ret = Config::Load())) {
            Log::Error("Config::Load failed: 0x%08x\n", ret);
            return ret;
        }
        
        if (R_FAILED(ret = intraFontInit())) {
            Log::Error("intraFontInit failed: 0x%08x\n", ret);
            return ret;
        }

        font = intraFontLoad("flash0:/font/ltn8.pgf", INTRAFONT_CACHE_ALL);
        G2D::FontSetStyle(1.f, WHITE, INTRAFONT_ALIGN_LEFT);
        
        // Font size cache
        for (int i = 0; i < 256; i++) {
            char character[2] = {0};
            character[0] = i;
            character[1] = '\0';
            font_size_cache[i] = intraFontMeasureText(font, character);
        }
        
        PSP_CTRL_ENTER = Utils::GetEnterButton();
        PSP_CTRL_CANCEL = Utils::GetCancelButton();
        language = Utils::GetLanguage();
        return 0;
    }
    
    void Exit(void) {
        if (sceUmdCheckMedium() != 0) {
            int ret = 0;
            
            if (R_FAILED(ret = sceUmdDeactivate(1, "disc0:")))
                Log::Error("sceUmdDeactivate(disc0) failed: 0x%x\n", ret);
        }
        

        pspUARTTerminate();
        intraFontUnload(font);
        Textures::Free();
        Utils::TermKernelDrivers();
        sceKernelExitGame();
    }
    
    static int ExitCallback(int arg1, int arg2, void *common) {
        running = false;
        return 0;
    }
    
    static int CallbackThread(SceSize args, void *argp) {
        int callback = 0;
        callback = sceKernelCreateCallback("ExitCallback", Services::ExitCallback, nullptr);
        sceKernelRegisterExitCallback(callback);
        sceKernelSleepThreadCB();
        return 0;
    } 

    static int SIOThread() {
            Log::Error("SETING UP THREAD\n");
        pspUARTInit(115200);
            Log::Error("SETUP\n");
            
        //while(running) {
        //    BTController::LoadControllerState();
        //    sceKernelDelayThread(10000);
       // }

        return 0;
    }

    static int SIOWrapper(SceSize args, void *argp) {
        Log::Error("Starting SIO Thread\n");
        return Services::SIOThread();
    }
    int SetupCallbacks(void) {
        Log::Clear();
        
        int thread = 0;

        if (R_SUCCEEDED(thread = sceKernelCreateThread("CallbackThread", Services::CallbackThread, 0x11, 0xFA0, 0, nullptr)))
            sceKernelStartThread(thread, 0, 0);
        
        return thread;
    }

    int SetupSIOThread(void) {

        int sioThread = 0;        
        
        if (R_SUCCEEDED(sioThread = sceKernelCreateThread("SIOThread", Services::SIOWrapper, 0x11, 0xFA0, 0, nullptr)))
        {
            Log::Error("IO THREAD CREATED\n");
           sceKernelStartThread(sioThread, 0, 0);
        }else
            Log::Error("Failed to create SIO thread\n");

        return sioThread;
    }
}

int main(int argc, char* argv[]) {
    Services::SetupCallbacks();
    Services::Init();
    Services::SetupSIOThread();
    GUI::RenderLoop();
    Services::Exit();
}
