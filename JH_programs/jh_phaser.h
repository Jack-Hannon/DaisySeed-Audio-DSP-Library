
#ifndef JH_PHASER_V2_H
#define JH_PHASER_V2_H

#include "jh_schroeder_apf.h"

// Description: Mono phaser class with tunable gain (mapped to LFO in stereo class), mix, and feedback
//              Program uses 16 schroeder all pass filters tuned to about the same gain to accumulate phase shifts without influencing frequency spectrum
//              By mixing the input (not phase shifted) signal with the phaser output, destructive interference occurs at frequencies where the phase shift
//              is 180 degrees * (2n + 1) and constructive interference where the phase shift is 180 * 2n. APF gain parameter alters position of the troughs
//              in the frequency domain
//              Feedback increases the 'strength' of the effect, and contributes resonant artifacts when feedback is high

class JH_phaser_v2
{
    public:
        JH_phaser_v2();
        void phaser_v2_preoperation(float gain_pot, float mix_pot, float fdbk_pot);
        float phaser_v2_operation(float input);

    private:
        float gain, mix, phaser_out, fdbk;
        schroeder_apf_v2 apf1, apf2, apf3, apf4,
                        apf5, apf6, apf7,apf8,
                        apf9, apf10, apf11, apf12,
                        apf13, apf14, apf15, apf16;
};

#endif
