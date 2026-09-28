
#include "jh_FractionalDelay2.h"
#include "jh_lfo.h"
#include "jh_utility.h"

#ifndef JH_FLANGER_H
#define JH_FLANGER_H

// Description: Stereo flanger effect based on the fractional_delay2 class (simplified, no filter)
//              Tunable modulation frequency, modulation depth/amplitude, and mix/feedback (effect 'strength')
//              Delay buffers of 380 to 480 samples work well for this effect

class jh_flanger
{
    public:
        jh_flanger(float *delay_bufferR, float *delay_bufferL, int buffer_size, float *lfo_LUT, int lfo_LUT_size);
        void flanger_preoperation(float freq_pot, float depth_pot, float mix_fdbk_pot);
        void flanger_operation(float input, float *temp_out);

    private:
        // independent right and left delay channels
        fractional_delay2 delayR, delayL; 

        JH_osc lfo;

        // mod freq controls lfo freq, mod_depth controls lfo amplitude, mix_fdbk controls the delay mix and 
        // feedback gain -> intensity of flanging
        float mod_freq, mod_depth, mix_fdbk;    
};

#endif