
#include "jh_flanger.h"

// Description: Stereo flanger effect based on the fractional_delay2 class (simplified, no filter)
//              Tunable modulation frequency, modulation depth/amplitude, and mix/feedback (effect 'strength')
//              Delay buffers of 380 to 480 samples work well for this effect


jh_flanger::jh_flanger(float *delay_bufferR, float *delay_bufferL, int buffer_size, float *lfo_LUT, int lfo_LUT_size) :
                delayR(delay_bufferR, buffer_size), delayL(delay_bufferL, buffer_size), lfo(lfo_LUT, lfo_LUT_size, 0, 0.1f, 64.0f, 48000)
{
    mod_freq = 0.2f;
    mod_depth = 0.0f;
    mix_fdbk = 0.0f;
}

void jh_flanger::flanger_preoperation(float freq_pot, float depth_pot, float mix_fdbk_pot)
{
    lfo.osc_preoperation(freq_pot, depth_pot);
    float lfo_val = lfo.osc_operation();

    // delay time bias + magnitude of sweep (lfo_val should come from a bipolar waveform restricted from -1 to 1)
    float delay_timeR = 0.547 + 0.423 * lfo_val;
    
    // slightly detune left and right channels to improve stereo image
    float delay_timeL = 0.542 + 0.427 * lfo_val; 

    // most extreme flanging when mix = 0.5 (equal parts input and delayed output)
    delayR.delay_preoperation(delay_timeR, (0.5 * mix_fdbk_pot), (mix_fdbk_pot* 2));
    delayL.delay_preoperation(delay_timeL, (0.5 * mix_fdbk_pot), (mix_fdbk_pot * 2));
}

void jh_flanger::flanger_operation(float input, float temp_out[2])
{
    temp_out[0] = delayR.delay_operation(input);
    temp_out[1] = delayL.delay_operation(input);
}