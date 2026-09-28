
#include "jh_apf_z1.h"
#include "jh_lfo.h"
#include "jh_utility.h"

#ifndef JH_PHASER16_H
#define JH_PHASER16_H

// Description: UPDATED mono phaser class with tunable gain (mapped to LFO in stereo class), mix, and feedback
//              ADDED gain_offset, modulation offset, saturation, and an lfo instance to support automatic modulation
//              Program uses 16 schroeder all pass filters tuned to about the same gain to accumulate phase shifts without influencing frequency spectrum
//              By mixing the input (not phase shifted) signal with the phaser output, destructive interference occurs at frequencies where the phase shift
//              is 180 degrees * (2n + 1) and constructive interference where the phase shift is 180 * 2n. APF gain parameter alters position of the troughs
//              in the frequency domain
//              Feedback increases the 'strength' of the effect, and contributes resonant artifacts when feedback is high

class phaser16
{
    public:
        phaser16(float *lfo_LUT, int lfo_lut_size, float gain_offset, float gain_modulation_offset, int lfo_phase);
        void phaser16_preoperation(float lfo_freq_pot, float lfo_depth_pot, float mix_fdbk_pot);
        float phaser16_operation(float input);
        
    private:
        float gain, mix, phaser_out, fdbk, g_offset, g_mod_offset;
        apf_z1  apf1, apf2, apf3, apf4,
                apf5, apf6, apf7,apf8,
                apf9, apf10, apf11, apf12,
                apf13, apf14, apf15, apf16;
        JH_osc lfo;
};

#endif