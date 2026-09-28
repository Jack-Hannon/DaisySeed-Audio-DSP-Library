
#include "jh_delay3.h"

#ifndef JH_STEREODELAY_H
#define JH_STEREODELAY_H

// Description: Wrapper class which supports a stereo delay/echo effect based on the delay3 class
//              Delay time and feedback are detuned between the left and right channel to broaden stereo image

class stereo_delay
{
    public:
        stereo_delay(float *delay_bufL, float *delay_bufR, const int buffer_size, float tangent_lut[10000]);
        void stereo_delay_preoperation(float delay_pot, float mix_pot, float fdbk_pot);
        void stereo_delay_operation(float input, float temp_out[2]);

    private:
        delay3 delayL, delayR;
};

#endif