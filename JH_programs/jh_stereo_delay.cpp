
#include "jh_stereo_delay.h"

// Description: Wrapper class which supports a stereo delay/echo effect based on the delay3 class
//              Delay time and feedback are detuned between the left and right channel to broaden stereo image


stereo_delay::stereo_delay(float *delay_bufL, float *delay_bufR, const int buffer_size, float tangent_lut[10000]) : delayL(delay_bufL, buffer_size, tangent_lut), delayR(delay_bufR, buffer_size, tangent_lut)
{}

void stereo_delay::stereo_delay_preoperation(float delay_pot, float mix_pot, float fdbk_pot)
{
    // Update independent, detuned delay lines to broaden stereo image
    delayL.delay3_preoperation(delay_pot, mix_pot, (fdbk_pot));
    delayR.delay3_preoperation((0.993 * delay_pot), mix_pot, (0.98 * fdbk_pot));
}

void stereo_delay::stereo_delay_operation(float input, float temp_out[2])
{
    temp_out[0] = delayL.delay3_operation(input);
    temp_out[1] = delayR.delay3_operation(input);
}

