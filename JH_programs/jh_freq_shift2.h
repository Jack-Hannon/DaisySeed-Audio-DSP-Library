
#include "jh_utility.h"
#include "jh_svf.h"

#ifndef JH_FREQ_SHIFT2_H
#define JH_FREQ_SHIFT2_H

// Description: Hilbert transform-based frequency shifter with a tunable frequency shift amount and sideband suppression control.
//              Implementation uses a fixed 501-tap Hilbert FIR.
//              Frequency shift range spans from -TOP_FREQ to +TOP_FREQ in Hz
//              Sideband mix parameter controls the relative weight of the sidebands (0 = DSB-SC, 1 = SSB-SC)

float hilbert_fir(float audio_buf[501], float input, int &write_idx);

class jh_freq_shifter2
{
    public:                                                                                                     
        jh_freq_shifter2(float* SIN_LUT, float* COS_LUT, int QUAD_SIZE, float hilbert_audio_buf[501], float input_audio_buf[251], float TOP_FREQ, float sample_rate, float tang_lut[10000]);
        
        // Per-block parameter updates
        void preoperation(float freq_pot, float SB_mix_pot);

        // Per-sample audio process
        float operation(float input);

    private:
        // Quadrature oscillator pair
        const float* sin_lut;
        const float* cos_lut;

        // maximum fractional index step size defines maximum frequency of quadrature oscillator pair
        // max_idx_step = (quadrature_buffer_length * top_frequency) / sample_rate)
        float max_idx_step;

        // Length of quadrature oscillator buffers
        int quad_size;

        float freq, bipolar_freq, SB_mix;

        // audio buffer for hilbert transform convolution 
        float* hilbert_buf;

        // In phase audio buffer
        float* input_buf;

        // input buffer for in-phase signal matches group delay of hilbert transform
        const int input_buf_size = 251;

        float quad_read_idx;
        int hilbert_write_idx, input_write_idx;

        svf_v2 filter;
};

#endif