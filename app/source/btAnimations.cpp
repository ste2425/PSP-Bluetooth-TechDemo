#include "btAnimations.h"

#include <pspkernel.h>

#include <pspsysevent.h>
#include <vector>
#include <array>
//#include <array.h>

#define ONE_MSEC (1000)

namespace BTAnimations {
    using Animation = std::vector<Frame>;
    static int currentPlayerOneFrame = -1;
    static int currentPlayerTwoFrame = -1;   
    static int currentPlayerOneAnimation = -1;
    static int currentPlayerTwoAnimation = -1;   

    static auto timer = sceKernelCreateVTimer("BTAnimationTimer", NULL);

    static std::array<Animation, 3> animations = {{
        // Animation 0: flash red
        { {255, 0, 0, 75}, {0, 0, 0, 125}, {255, 0, 0, 125}, {0, 0, 0, 125}, {255, 0, 0, 125} }
    }};
    
    SceUInt timer_handler(SceUID uid, SceKernelSysClock *requested, SceKernelSysClock *actual, void *common) {
        auto animation = animations[currentPlayerOneAnimation];

        if (currentPlayerOneFrame >= (int)animation.size())
        {
            currentPlayerOneFrame = -1;
            currentPlayerOneAnimation = -1;

            BTController::SetColour(0x00, 0, 0, 0);

            return 0;
        } else {        
            auto frame = animation[currentPlayerOneFrame];

            BTController::SetColour(0x00, frame.red, frame.green, frame.blue);

            currentPlayerOneFrame++;

            //SceKernelSysClock sc;
            //sceKernelGetVTimerTime(timer, &sc);
            //sc.low += frame.duration;
            return frame.duration * ONE_MSEC;
        }
    }

    void Animate()
    {
        
    }

    void StopAnimation() 
    {

    }

    void StartAnimation(uint8_t animation)
    {
        currentPlayerOneAnimation = animation;
        currentPlayerOneFrame = 0;
        
        auto anim = animations[currentPlayerOneAnimation];
        auto frame = anim[currentPlayerOneFrame];

        SceKernelSysClock sc;
        sceKernelGetVTimerTime(timer, &sc);
        sc.low += frame.duration * ONE_MSEC;

        BTController::SetColour(0x00, frame.red, frame.green, frame.blue);
        //void* timer_addr = GUI::timer_handler;
        auto ret = sceKernelSetVTimerHandler(timer, &sc, BTAnimations::timer_handler, NULL);
        
        if (ret == 0) {
            sceKernelStartVTimer(timer);
        }
    }
}