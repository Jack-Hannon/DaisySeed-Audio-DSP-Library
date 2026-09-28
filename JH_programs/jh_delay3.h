
#include "jh_utility.h"
#include "jh_svf.h"

#ifndef JH_DELAY3_H
#define JH_DELAY3_H

// Description: Fractional delay line with tunable delay length, mix amount, and feedback
//              Includes a low pass filter and saturation function in the feedback path
//              See C++ file to alter cutoff frequency and filter type (LP, HP, BP)
//              At 48kHz sample rate, maximum delay time = (buffer_size / 48000)

class delay3
{
    public:
        delay3(float *delay_buf, const int buffer_size, float tangent_lut[10000]);

        // Tunable parameters updated once per audio block
        void delay3_preoperation(float delay_pot, float mix_pot, float fdbk_pot);

        // Per-sample processing function
        float delay3_operation(float input);

    private:
        float delT, mix, fdbk;
        float *delay_buffer;
        int write_idx;
        int buf_size;
        float del_output, final_output;
        svf_v2 filter;
};

#endif