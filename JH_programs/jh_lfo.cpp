
#include "jh_lfo.h"

// Description: Low frequency oscillator class based on look up table
//              Tunable frequency and amplitude
//              wave_lut is the waveform look up table. I recommend passing a buffer with one period of the desired waveform,
//              though the class will work as intended provided a buffer with an integer number of wavelengths.
//              A frequency range (low_freq-high_freq) of ~ 0.1 - 20 Hz works well with a 10,000 sample wave_lut buffer (buf_size)
//              phase can be set to offset several instances of the LFO class. Input phase in degrees from 0 to 360


JH_osc::JH_osc(float* wave_lut, int buf_size, int phase, float low_freq, float high_freq, float sr)
{
    waveform_lut = wave_lut;
    lut_size = buf_size;

    // define bounds on the index increment range
    low_increment = low_freq * buf_size / sr;
    high_increment = high_freq * buf_size / sr;

    // fractional index increment parameter controlled via user controlled potentiometer
    lut_idx_increment = 1.0f;

    amplitude = 0.5f;

    // initial phase
    int corrected_phase = ((phase % 360) + 360) % 360;
    lut_idx = (static_cast<float>(corrected_phase) / 360.0f) * (buf_size - 1);
}

void JH_osc::osc_preoperation(float freq_input, float amp_input)
{
    // slew amplitude and frequency potentiometer controls
    amplitude = amplitude * 0.95 + 0.05 * amp_input;
    lut_idx_increment = 0.95 * lut_idx_increment + 0.05 * (low_increment + (high_increment - low_increment) * freq_input);      //calculate idx_increment range to support specified frequency range
}

float JH_osc::osc_operation()
{
    // use linear interpolation to define LFO output
    int lower_idx = static_cast<int>(lut_idx);
    float idx_fraction = lut_idx - lower_idx;       
    int upper_idx = (lower_idx + 1) % lut_size;
    float lut_output = amplitude * ((1-idx_fraction) * waveform_lut[lower_idx] + idx_fraction * waveform_lut[upper_idx]);

    lut_idx = ((lut_idx + lut_idx_increment) < lut_size) ? (lut_idx + lut_idx_increment) : (lut_idx + lut_idx_increment - lut_size);
    
    return lut_output;
}
