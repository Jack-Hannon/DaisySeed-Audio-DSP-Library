
#include "jh_stereo_phaser16.h"

// Description: Stereo phaser class based on the mono phaser16 class. Modulation and gain offsets included to control detuning of the gain
//              parameter across the left and right channels.
//              Setting the left and right phase to 0 and 180 respectively leads to a nice crossfading effect


stereo_phaser16::stereo_phaser16(float *lfo_lut, const int lfo_lut_size, float gain_offsetL, float gain_offsetR, float gain_modulation_offsetL, float gain_modulation_offsetR, int lfo_phaseL, int lfo_phaseR) :
                phaserR(lfo_lut, lfo_lut_size, gain_offsetR, gain_modulation_offsetR, lfo_phaseR), phaserL(lfo_lut, lfo_lut_size, gain_offsetL, gain_modulation_offsetL, lfo_phaseL) {}

                void stereo_phaser16::stereo_phaser16_preoperation(float LFO_freq_pot, float LFO_depth_pot, float MIX_fdbk_pot)
{
    phaserR.phaser16_preoperation(LFO_freq_pot, LFO_depth_pot, MIX_fdbk_pot);       // detuned with separate offset inputs in constructor
    phaserL.phaser16_preoperation(LFO_freq_pot, LFO_depth_pot, MIX_fdbk_pot);
}

void stereo_phaser16::stereo_phaser16_operation(float input, float temp_out[2])
{
    temp_out[0] = phaserL.phaser16_operation(input);
    temp_out[1] = phaserR.phaser16_operation(input);
}
