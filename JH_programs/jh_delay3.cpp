
#include "jh_delay3.h"

// Description: Fractional delay line with tunable delay length, mix amount, and feedback
//              Includes a low pass filter and saturation function in the feedback path
//              Alter cutoff frequency and filter type (LP, HP, BP) here
//              At 48kHz sample rate, maximum delay time = (buffer_size / 48000)


delay3::delay3(float *delay_buf, const int buffer_size, float tangent_lut[10000]) : filter(tangent_lut)
{
    delay_buffer = delay_buf;
    write_idx = 0;
    buf_size = buffer_size;     // maximum delay time (s) = (buffer_size / sample rate)
    delT = 0.0f;
    mix = 0.0f;
    fdbk = 0.0f;
    del_output = 0.0f;
    final_output = 0.0f;   
}

void delay3::delay3_preoperation(float delay_pot, float mix_pot, float fdbk_pot)
{
    // update tunable parameters based on potentiometer values
    delT = delT * 0.999 + 0.001 * delay_pot;     
    mix = mix * 0.95 + 0.05 * mix_pot;       
    fdbk = 0.95 * fdbk + 0.053 * ( (fdbk_pot > 0.12) ? (fdbk_pot - 0.1) : 0.0f );

    // low pass filter with cutoff ~ 4 kHz
    // svf_v2_preoperation(cutoff frequency, resonance, filter type (< 0.33 = LPF, 0.33 - 0.67 = BPF, > .67 = HPF))
    // see jh_svf.cpp for precise tuning
    filter.svf_v2_preoperation(0.75, 0.3, 0.1);             
}

float delay3::delay3_operation(float input)
{
    delay_buffer[write_idx] = (soft_clip(del_output, 1.0)) * fdbk + input;

    // calculate fractional read index with cubic mapping between potentiometer value and delay time
    float read_idx_raw = (write_idx + ((buf_size - 10) - (buf_size - 20) * delT * delT * delT));
    float read_idx = (read_idx_raw < buf_size) ? read_idx_raw : (read_idx_raw - buf_size);

    // compute integer indices for hermite interpolation window
    int idx1 = static_cast<int>(read_idx);
    int idx0 = ( idx1 - 1 >= 0 ) ? idx1 - 1 : buf_size - 1;
    int idx2 = ( idx1 + 1 < buf_size ) ? idx1 + 1 : 0;
    int idx3 = ( idx2 + 1 < buf_size ) ? idx2 + 1 : 0;

    float read_val = 0.0f;

    read_val = hermite_interpolation1(delay_buffer[idx0], delay_buffer[idx1],delay_buffer[idx2], delay_buffer[idx3], idx1, read_idx);

    write_idx = ( write_idx + 1 < buf_size) ? write_idx + 1 : 0;
    
    // adjust ratio of raw output vs filtered output to taste (0.85 : 0.1)
    del_output = 0.1 * filter.svf_v2_operation(read_val) + 0.85 * read_val;
    final_output = mix * del_output + (1 - mix) * input;
    return final_output;
}
