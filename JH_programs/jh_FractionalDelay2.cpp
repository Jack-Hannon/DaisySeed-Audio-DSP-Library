
#include "jh_FractionalDelay2.h"

// Description: Lighter fractional delay class without a feedback path filter. Used in the reverb6 and flanger effects
//              See jh_delay3.h for a discussion on tuning fixed parameters


fractional_delay2::fractional_delay2(float *delay_buf, int buffer_size)
{
    delay_buffer = delay_buf;
    write_idx = 0;
    buf_size = buffer_size;
    delT = 0.0f;
    mix = 0.0f;
    fdbk = 0.0f;
    output = 0.0f;
}

void fractional_delay2::delay_preoperation(float delay_pot, float mix_pot, float fdbk_pot)
{
    // slew potentiometer signals to remove noise
    delT = delT * 0.995 + 0.005 * delay_pot;     
    mix = mix * 0.95 + 0.05 * mix_pot;       
    fdbk = 0.95 * fdbk + 0.04 * ( (fdbk_pot > 0.12) ? (fdbk_pot - 0.1) : 0.0f );
}     

float fractional_delay2::delay_operation(float input)
{
    // write current input and attenuated delay output back into the delay line
    delay_buffer[write_idx] = (soft_clip(output, 1.3)) * fdbk + input;

    // compute fractional read index
    float read_idx_raw = (write_idx + ((buf_size - 10) - (buf_size - 20) * delT));
    float read_idx = (read_idx_raw < buf_size) ? read_idx_raw : (read_idx_raw - buf_size);

    // linear interpolation
    int idx1 = static_cast<int>(read_idx);
    int idx2 = ( idx1 + 1 < buf_size ) ? idx1 + 1 : 0;
    float read_val = linear_interpolation(delay_buffer[idx1], delay_buffer[idx2], (read_idx - idx1));

    // increment write index in circular buffer
    write_idx = ( write_idx + 1 < buf_size) ? write_idx + 1 : 0;

    output = read_val * mix + (1 - mix) * input;
    return output;
}