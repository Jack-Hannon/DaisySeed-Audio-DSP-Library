
#include "jh_FractionalDelay.h"

// Description: Early fractional delay effect using linear interpolation and a feedback filter.
//              See jh_delay3.h for a discussion on tuning fixed parameters.


fractional_delay::fractional_delay(float *delay_buf, int buffer_size, bool modulation, float tang_LUT[10000]) : svf1(tang_LUT)
{
    delay_buffer = delay_buf;
    mod_switch = modulation;
    write_idx = 0;
    buf_size = buffer_size;
    delT = 0.0f;
    mix = 0.0f;
    fdbk = 0.0f;
    output = 0.0f;
}

void fractional_delay::delay_preoperation(float pot0, float pot1, float pot2)
{
    // fixed LPF with ~2800 Hz cutoff and 2.7 Q (resonance) in feedback path
    svf1.svf_v2_preoperation(0.2, 0.34, 0.1);

    // delay time
    delT = delT * 0.995 + 0.005 * pot0;

    // mix between dry and processed signal. Capped at 0.9
    mix = mix * 0.95 + 0.045 * pot1;   

    // Feedback within delay line. Capped at 0.8 to prevent self oscillation
    fdbk = 0.95 * fdbk + 0.04 * ( (pot2 > 0.1) ? (pot2 - 0.05) : 0.0f );
}

float fractional_delay::delay_operation(float input)
{
    // write input and attenuated delay output (previous) back to write index
    delay_buffer[write_idx] = output * fdbk + input;

    // compute fractional read index
    float read_idx_raw = (write_idx + ((buf_size - 10) - (buf_size - 50) * delT));
    float read_idx = (read_idx_raw < buf_size) ? read_idx_raw : (read_idx_raw - buf_size);

    // linear interpolation
    int idx1 = static_cast<int>(read_idx);
    int idx2 = (static_cast<int>(read_idx + 1)) % buf_size;
    float idx_difference = read_idx - idx1;
    float read_val = (1 - idx_difference) * delay_buffer[idx1] + idx_difference * delay_buffer[idx2];

    write_idx = (write_idx + 1) % buf_size;
    float del_output = read_val * mix + (1 - mix) * input;

    // output is a mix of the delayed output and a damped (LPFed) version
    output = 0.6 * del_output + 0.4 * svf1.svf_v2_operation(del_output);
    return output;
}
