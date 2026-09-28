
#include "jh_phaser16.h"

// Description: UPDATED mono phaser class with tunable gain (mapped to LFO in stereo class), mix, and feedback
//              ADDED gain_offset, modulation offset, saturation, and an lfo instance to support automatic modulation
//              Program uses 16 schroeder all pass filters tuned to about the same gain to accumulate phase shifts without influencing frequency spectrum
//              By mixing the input (not phase shifted) signal with the phaser output, destructive interference occurs at frequencies where the phase shift
//              is 180 degrees * (2n + 1) and constructive interference where the phase shift is 180 * 2n. APF gain parameter alters position of the troughs
//              in the frequency domain
//              Feedback increases the 'strength' of the effect, and contributes resonant artifacts when feedback is high


phaser16::phaser16(float *lfo_LUT, int lfo_lut_size, float gain_offset, float gain_modulation_offset, int lfo_phase): apf1(), apf2(), apf3(), apf4(),apf5(), apf6(), apf7(), apf8(),
                                                    apf9(), apf10(), apf11(), apf12(),apf13(), apf14(), apf15(), apf16(),lfo(lfo_LUT, lfo_lut_size, lfo_phase, 0.1f, 64.0f, 48000)
{
    gain = 0.55f;
    mix = 0.0f;   
    fdbk = 0.0f;
    phaser_out = 0.0f;   
    g_offset = (gain_offset < 0.1) ? gain_offset : 0.09;
    g_mod_offset = (gain_modulation_offset + gain_offset < 0.1) ? gain_modulation_offset : 0;            
}

void phaser16::phaser16_preoperation(float lfo_freq_pot, float lfo_depth_pot, float mix_fdbk_pot)
{
    // update lfo signal
    lfo.osc_preoperation(lfo_freq_pot, lfo_depth_pot);
    float lfo_out = lfo.osc_operation();
    
    // slew user controlled parameters
    gain = 0.55 + g_offset + (0.35 + g_mod_offset) * lfo_out;
    mix = mix * 0.95 + 0.025 * mix_fdbk_pot;
    fdbk = 0.95 * fdbk + 0.05 * mix_fdbk_pot;

    apf1.apf_preoperation(0.991 * gain);        // Slightly detune the gain parameters of each APF instance (qualitatively tuned)
    apf2.apf_preoperation(0.977 * gain);
    apf3.apf_preoperation(0.981 * gain);
    apf4.apf_preoperation(0.969 * gain);
    apf5.apf_preoperation(0.978 * gain);
    apf6.apf_preoperation(0.963 * gain);
    apf7.apf_preoperation(0.994 * gain);
    apf8.apf_preoperation(0.977 * gain);
    apf9.apf_preoperation(0.985 * gain);
    apf10.apf_preoperation(0.955 * gain);
    apf11.apf_preoperation(gain);
    apf12.apf_preoperation(0.969 * gain);
    apf13.apf_preoperation(0.976 * gain);
    apf14.apf_preoperation(0.987 * gain);
    apf15.apf_preoperation(gain);
    apf16.apf_preoperation(0.953 * gain);
}
float phaser16::phaser16_operation(float input)
{
    // mix audio input and phaser output to form phaser input signal
    float phaser_in = input * (1 - 0.3 * fdbk) + soft_clip3(phaser_out * fdbk, 1.0);

    // process through serial APF chain
    float apf1_out = apf1.apf_operation(phaser_in);
    float apf2_out = apf2.apf_operation(apf1_out);
    float apf3_out = apf3.apf_operation(apf2_out);
    float apf4_out = apf4.apf_operation(apf3_out);
    float apf5_out = apf5.apf_operation(apf4_out);
    float apf6_out = apf6.apf_operation(apf5_out);
    float apf7_out = apf7.apf_operation(apf6_out);
    float apf8_out = apf8.apf_operation(apf7_out);
    float apf9_out = apf9.apf_operation(apf8_out);
    float apf10_out = apf10.apf_operation(apf9_out);
    float apf11_out = apf11.apf_operation(apf10_out);
    float apf12_out = apf12.apf_operation(apf11_out);
    float apf13_out = apf13.apf_operation(apf12_out);
    float apf14_out = apf14.apf_operation(apf13_out);
    float apf15_out = apf15.apf_operation(apf14_out);
    phaser_out = apf16.apf_operation(apf15_out);
    
    return ( phaser_out * mix + input * (1 - mix) );
}
