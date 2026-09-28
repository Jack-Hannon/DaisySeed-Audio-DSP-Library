
#include "jh_FractionalDelay2.h"
#include "jh_lfo.h"
#include "jh_utility.h"

#ifndef JH_REVERB4_H
#define JH_REVERB4_H

// Description: Experimental Reverb program with time modulated delay lines, APF diffusers, and Hadamard matrix mixing
//              Tunable controls for delay time, mix, and feedback

// Schroeder All Pass filter class with a tunable delay line
class apf_zm_rev
{
    public:
        apf_zm_rev(float *delay_line, int delay_line_size);
        void apf_zm_rev_preoperation(float gain_pot, float delay_pot);
        float apf_zm_rev_operation(float input);

    private:
        int buf_size;
        int write_idx;
        float read_idx;
        float g, delay, delay_samples;     // both proportions from 0 to 1   
        float *apf_buf;
};


class reverb_v4
{
    public:
        reverb_v4(float *delA_buf, float *delB_buf, float *delC_buf, float *delD_buf, int del_buf_size, 
                    float *apf1_buf, float *apf2_buf, float *apf3_buf, int apf_num_buf_size, 
                    float *apfA_buf, float *apfB_buf, float *apfC_buf, float *apfD_buf, int apf_fdbk_buf_size, 
                    float *waveform_LUT, int waveform_LUT_size);
        void reverb_v4_preoperation(float delay_pot, float mix_pot, float feedback_pot);
        void reverb_v4_operation(float input, float *temp_out);             // temp out should be a 2 item array temp_out[0] = out[0][i] outside of function, temp_out[1] = out[1][i]
    
    private:
        fractional_delay2 delA, delB, delC, delD;
        apf_zm_rev apf1, apf2, apf3, apfA, apfB, apfC, apfD;    // apfs denoted by number are at input for diffusion, apfs with letter are in feedback loop 
        JH_osc lfoA, lfoB, lfoC, lfoD;
        float H1_inA, H1_inB, H1_inC, H1_inD;       // H1 is feedforward Hadamard matrix
        float H1_outA, H1_outB, H1_outC, H1_outD;
        float apfA_out, apfB_out, apfC_out, apfD_out;
        float del_time, mix, fdbk;
};





#endif