
#include "jh_phaser16.h"


// Description: Stereo phaser class based on the mono phaser16 class. Modulation and gain offsets included to control detuning of the gain
//              parameter across the left and right channels.
//              Setting the left and right phase to 0 and 180 respectively leads to a nice crossfading effect

class stereo_phaser16
{
    public:
        stereo_phaser16(float *lfo_lut, const int lfo_lut_size, float gain_offsetL, float gain_offsetR, float gain_modulation_offsetL, float gain_modulation_offsetR, int lfo_phaseL, int lfo_phaseR);
        void stereo_phaser16_preoperation(float LFO_freq_pot, float LFO_depth_pot, float MIX_fdbk_pot);
        void stereo_phaser16_operation(float input, float temp_out[2]);
        
    private:
        phaser16 phaserR, phaserL;
};