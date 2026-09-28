
#include "jh_reverb6.h"

// Description: Reverb program based on Gerzon reverb topology. This implementation uses two parallel gerzon structures,
//              in -> parallel all pass filters (diffusers) -> parallel delay lines -> hadamard matrix -> Stereo output
//              The hadamard matrix output is fed into the input of the delay lines associated with the 2nd 'gerzon structure'
//              and the hadamard matrix output of the 2nd structure is fed back into the input of the 1st delay lines.
//              The all pass filters and parallel delays are tuned to have 'prime' delay lengths, so that echos do not overlap,
//              producing a dominant mode/tone.
//              The reverb has tunable mix, feedback, and delay length parameters. The mix control manipulates the ratio between
//              the dry and processed signals. the feedback control adjusts the amount of the output that is fed back into the input
//              delay lines. A larger feedback value leads to a longer reverb tail. The delay time control adjusts the 'size' of
//              the virtual room by adjusting all of the delay lengths (dominant echos) simultaneously.


jh_reverb6::jh_reverb6(float *apf1_buf, float *apf2_buf, float *apf3_buf, float *apf6_buf, float *apf7_buf, float *apf8_buf, int apf_buf_size,
                    float *del1_buf, float *del2_buf, float *del3_buf, float *del4_buf, float *del5_buf, float *del6_buf, float *del7_buf, float *del8_buf, 
                    int del_buf_size) : apf1(apf1_buf, apf_buf_size), apf2(apf2_buf, apf_buf_size), apf3(apf3_buf, apf_buf_size), 
                    apf6(apf6_buf, apf_buf_size), apf7(apf7_buf, apf_buf_size), apf8(apf8_buf, apf_buf_size), del1(del1_buf, del_buf_size), del2(del2_buf, del_buf_size), 
                    del3(del3_buf, del_buf_size), del4(del4_buf, del_buf_size), del5(del5_buf, del_buf_size), del6(del6_buf, del_buf_size), del7(del7_buf, del_buf_size), 
                    del8(del8_buf, del_buf_size)
{
    // initialize tunable parameters
    delay_time = 0.5f;
    mix = 0.0f;
    fdbk = 0.0f;  

    // initialize Hadamard matrix states
    HA_out1 = 0.0f, HA_out2 = 0.0f, HA_out3 = 0.0f, HA_out4 = 0.0f;
    HB_out1 = 0.0f, HB_out2 = 0.0f, HB_out3 = 0.0f, HB_out4 = 0.0f;          
}

void jh_reverb6::reverb6_preoperation(float delay_pot, float mix_pot, float fdbk_pot)
{
    delay_time = 1 + 1.5 * delay_pot; 
    mix = 0.95 * mix + 0.05 * mix_pot;
    fdbk = 0.95 * fdbk + 0.05 * fdbk_pot;

    // detuned all pass filter diffusers (length 1000)
    // gerzon stage 1
    apf1.apf_zm_rev_preoperation(0.6, 0.149);
    apf2.apf_zm_rev_preoperation(0.43, 0.184);
    apf3.apf_zm_rev_preoperation(0.9, 0.236);
    // gerzon stage 2
    apf6.apf_zm_rev_preoperation(0.23, 0.296);
    apf7.apf_zm_rev_preoperation(0.34, 0.371);
    apf8.apf_zm_rev_preoperation(0.7, 0.465);

    // scaled delay line controls based on shared delay time parameter
    float del1_time = 0.077 * delay_time;
    float del2_time = 0.161 * delay_time;
    float del3_time = 0.095 * delay_time;
    float del4_time = 0.190 * delay_time;

    // Update independent delay lines (length 9600 each)
    del1.delay_preoperation(del1_time, 1, 0);       
    del2.delay_preoperation(del2_time, 1, 0);
    del3.delay_preoperation(del3_time, 1, 0);
    del4.delay_preoperation(del4_time, 1, 0);

    float del5_time = 0.114 * delay_time;
    float del6_time = 0.223 * delay_time;
    float del7_time = 0.135 * delay_time;
    float del8_time = 0.250 * delay_time;

    del5.delay_preoperation(del5_time, 1, 0);
    del6.delay_preoperation(del6_time, 1, 0);
    del7.delay_preoperation(del7_time, 1, 0);
    del8.delay_preoperation(del8_time, 1, 0);
}

void jh_reverb6::reverb6_operation(float input, float temp_out[2])
{
    // Stage 1 delay inputs
    float del1_in = apf1.apf_zm_rev_operation(input) + HB_out4 * fdbk;
    float del2_in = apf2.apf_zm_rev_operation(input) + HB_out3 * fdbk;
    float del3_in = apf3.apf_zm_rev_operation(input) + HB_out2 * fdbk;
    float del4_in = input + HB_out1 * fdbk;

    // Stage 2 delay inputs
    float del5_in = input + HA_out4 * fdbk;
    float del6_in = apf6.apf_zm_rev_operation(input) + HA_out3 * fdbk;
    float del7_in = apf7.apf_zm_rev_operation(input) + HA_out2 * fdbk;
    float del8_in = apf8.apf_zm_rev_operation(input) + HA_out1 * fdbk;

    // Stage 1 Hadamard mixing and saturation
    float HA_in1 = soft_clip3((del1.delay_operation(del1_in)), 1.0);
    float HA_in2 = soft_clip3((del2.delay_operation(del2_in)), 1.0);
    float HA_in3 = soft_clip3((del3.delay_operation(del3_in)), 1.0);
    float HA_in4 = soft_clip3((del4.delay_operation(del4_in)), 1.0);

    // Stage 2 Hadamard mixing and saturation
    float HB_in1 = soft_clip3((del5.delay_operation(del5_in)), 1.0);
    float HB_in2 = soft_clip3((del6.delay_operation(del6_in)), 1.0);
    float HB_in3 = soft_clip3((del7.delay_operation(del7_in)), 1.0);
    float HB_in4 = soft_clip3((del8.delay_operation(del8_in)), 1.0);

    // Scale Hadamard outputs to maintain unitary property
    HA_out1 = 0.5 * (HA_in1 + HA_in2 + HA_in3 + HA_in4);
    HA_out2 = 0.5 * (HA_in1 - HA_in2 + HA_in3 - HA_in4);
    HA_out3 = 0.5 * (HA_in1 + HA_in2 - HA_in3 - HA_in4);
    HA_out4 = 0.5 * (HA_in1 - HA_in2 - HA_in3 + HA_in4);

    HB_out1 = 0.5 * (HB_in1 + HB_in2 + HB_in3 + HB_in4);
    HB_out2 = 0.5 * (HB_in1 - HB_in2 + HB_in3 - HB_in4);
    HB_out3 = 0.5 * (HB_in1 + HB_in2 - HB_in3 - HB_in4);
    HB_out4 = 0.5 * (HB_in1 - HB_in2 - HB_in3 + HB_in4);

    // Stereo outputs
    temp_out[0] = mix * HA_out2 + (1 - mix) * input;
    temp_out[1] = mix * HB_out2 + (1 - mix) * input;
}