
#include "jh_reverb4.h"
#include "jh_FractionalDelay2.h"
#include "jh_lfo.h"
#include "jh_utility.h"

// Description: Experimental Reverb program with time modulated delay lines, APF diffusers, and Hadamard matrix mixing
//              Tunable controls for delay time, mix, and feedback


apf_zm_rev::apf_zm_rev(float *delay_line, int delay_line_size)
{
    g = 0.5f;
    apf_buf = delay_line;
    buf_size = delay_line_size;
    write_idx = 0;
    read_idx = 0.5f * delay_line_size;
    delay = 0.5 * delay_line_size;
    delay_samples = 0.0f;
}

void apf_zm_rev::apf_zm_rev_preoperation(float gain_pot, float delay_pot)
{
    g = 0.95 * g + 0.05 * gain_pot;
    delay = 0.99 * delay + 0.01 * delay_pot;
    delay_samples = delay * (buf_size - 10);
}

float apf_zm_rev::apf_zm_rev_operation(float input)
{
    read_idx = write_idx + 1 + (buf_size - delay_samples);
    while (read_idx > buf_size) {read_idx -= buf_size;}
    if (read_idx < 0) read_idx = 0.0f;
    int prev_idx = (int) read_idx;
    int next_idx = (prev_idx + 1 < buf_size) ? prev_idx + 1 : 0;
    float del_out = linear_interpolation(apf_buf[prev_idx], apf_buf[next_idx], (read_idx - prev_idx));
    float del_in = input - g * del_out;
    float out = del_out + g * del_in;
    write_idx++;
    if (write_idx >= buf_size) write_idx = 0;
    return out;       
}

// Reverb program inspired by Gerzon reverberation algorithm
// Independent, time-varying delay lines are mixed through a hadamard matrix in the feedback path
// Schroeder APFs diffuse signal to mitigate prominent resonances
reverb_v4::reverb_v4(float *delA_buf, float *delB_buf, float *delC_buf, float *delD_buf, int del_buf_size, 
                    float *apf1_buf, float *apf2_buf, float *apf3_buf, int apf_num_buf_size, 
                    float *apfA_buf, float *apfB_buf, float *apfC_buf, float *apfD_buf, int apf_fdbk_buf_size, 
                    float *waveform_LUT, int waveform_LUT_size) : delA(delA_buf, del_buf_size), delB(delB_buf, del_buf_size), 
                    delC(delC_buf, del_buf_size), delD(delD_buf, del_buf_size), apf1(apf1_buf, apf_num_buf_size), apf2(apf2_buf, apf_num_buf_size), 
                    apf3(apf3_buf, apf_num_buf_size), apfA(apfA_buf, apf_fdbk_buf_size), apfB(apfB_buf, apf_fdbk_buf_size), apfC(apfC_buf, apf_fdbk_buf_size), apfD(apfD_buf, apf_fdbk_buf_size), 
                    lfoA(waveform_LUT, waveform_LUT_size, 13, 0.1, 1, 48000), lfoB(waveform_LUT, waveform_LUT_size, 83, 0.17, 0.93, 48000), lfoC(waveform_LUT, waveform_LUT_size, 121, 0.19, 1.07, 48000), 
                    lfoD(waveform_LUT, waveform_LUT_size, 273, 0.15, 1.14, 48000)
{
    // Initialize hadamard matrix states
    H1_inA = 0.0f, H1_inB = 0.0f, H1_inC = 0.0f, H1_inD = 0.0f;
    H1_outA = 0.0f, H1_outB = 0.0f, H1_outC = 0.0f, H1_outD = 0.0f;

    // initialize APF states
    apfA_out = 0.0f, apfB_out = 0.0f, apfC_out = 0.0f, apfD_out = 0.0f;

    // initialize user controlled parameters
    del_time = 0.0f, mix = 0.0f, fdbk = 0.0f;
}

void reverb_v4::reverb_v4_preoperation(float delay_pot, float mix_pot, float feedback_pot)
{
    del_time = 1 + 3 * delay_pot;          
    mix = 0.95 * mix + 0.05 * mix_pot;
    fdbk = 0.95 * fdbk + 0.05 * feedback_pot;

    lfoA.osc_preoperation(0.87, 1);
    lfoB.osc_preoperation(0.72, 1);
    lfoC.osc_preoperation(0.63, 1);
    lfoD.osc_preoperation(0.93, 1);

    // Define input apfs to have buffer size = 1000; delays = 3.1 ms, 7.2 ms, 10.5 ms
    apf1.apf_zm_rev_preoperation(0.5, 0.15);        
    apf2.apf_zm_rev_preoperation(0.43, 0.346);
    apf3.apf_zm_rev_preoperation(0.57, 0.504);

    float lfoA_out = lfoA.osc_operation();
    float lfoB_out = lfoB.osc_operation();
    float lfoC_out = lfoC.osc_operation();
    float lfoD_out = lfoD.osc_operation();

    float apfT_A = 0.17 * del_time + 0.07 * del_time * lfoC_out;
    float apfT_B = 0.27 * (del_time/2.0f) + 0.09 * del_time * lfoD_out;
    float apfT_C = 0.613 + 0.063 * del_time * lfoA_out;
    float apfT_D = 0.736  + 0.06 * del_time * lfoB_out;

    // Define feedback apf buffer size = 600. delays = 2.1 ms, 3.4 ms, 7.7 ms, 9.2 ms
    apfA.apf_zm_rev_preoperation(0.61, apfT_A);        
    apfB.apf_zm_rev_preoperation(0.51, apfT_B);
    apfC.apf_zm_rev_preoperation(0.41, apfT_C);
    apfD.apf_zm_rev_preoperation(0.90, apfT_D);

    // potentiometer signal scales the central delay time (relationships hold between delay lines) and the modulation depth
    float delT_A = 0.0436 * del_time + 0.033 * del_time * lfoA_out;     
    float delT_B = 0.0689 * del_time + 0.045 * del_time * lfoB_out;
    float delT_C = 0.0986 * del_time + 0.063 * del_time * lfoC_out;
    float delT_D = 0.1486 * del_time + 0.094 * del_time * lfoD_out;

    // all delay lines should have buffer length = 9600 ( max of 200 ms delay)
    delA.delay_preoperation(delT_A, 0.93, 0);         
    delB.delay_preoperation(delT_B, 1, 0);
    delC.delay_preoperation(delT_C, 1, 0);
    delD.delay_preoperation(delT_D, 0.96, 0);
}

 void reverb_v4::reverb_v4_operation(float input, float temp_out[2])
{
    // outputs of diffuser apfs + Hadamard 2 (feedback matrix) = delay line inputs
    float inB = apf1.apf_zm_rev_operation(input) + apfB_out;       
    float inC = apf2.apf_zm_rev_operation(input) + apfC_out;
    float inD = apf3.apf_zm_rev_operation(input) + apfD_out;

    H1_inA = delA.delay_operation(input + apfA_out);
    H1_inB = delB.delay_operation(inB);
    H1_inC = delC.delay_operation(inC);
    H1_inD = delD.delay_operation(inD);

    H1_outA = 0.5 * (H1_inA + H1_inB + H1_inC + H1_inD);
    H1_outB = 0.5 * (H1_inA - H1_inB + H1_inC - H1_inD);
    H1_outC = 0.5 * (H1_inA + H1_inB - H1_inC - H1_inD);
    H1_outD = 0.5 * (H1_inA - H1_inB - H1_inC + H1_inD);

    apfA_out = apfA.apf_zm_rev_operation(H1_outA * fdbk);
    apfB_out = apfB.apf_zm_rev_operation(H1_outB * fdbk);
    apfC_out = apfC.apf_zm_rev_operation(H1_outC * fdbk);
    apfD_out = apfD.apf_zm_rev_operation(H1_outD * fdbk);

    temp_out[0] = H1_outA * mix + input * (1 - mix);
    temp_out[1] = H1_outB * mix + input * (1 - mix);        
}

