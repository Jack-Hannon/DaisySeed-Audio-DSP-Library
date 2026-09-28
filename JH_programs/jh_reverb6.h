
#include "jh_FractionalDelay2.h"
#include "jh_lfo.h"
#include "jh_utility.h"
#include "jh_reverb4.h"         // contains all pass filter class with variable delay length (z^-m)

#ifndef JH_REVERB6_H
#define JH_REVERB6_H

// Description: Reverb program based on gerzon reverb topology. This implementation uses two parallel gerzon structures,
//              in -> parallel all pass filters (diffusers) -> parallel delay lines -> hadamard matrix -> left or right out
//              The hadamard matrix output is fed into the input of the delay lines associated with the 2nd 'gerzon structure'
//              and the hadamard matrix output of the 2nd structure is fed back into the input of the 1st delay lines.
//              The all pass filters and parallel delays are tuned to have 'prime' delay lengths, so that echos do not overlap,
//              producing a dominant mode/tone.
//              The reverb has tunable mix, feedback, and delay length parameters. The mix control manipulates the ratio between
//              the dry and reverb signals. the feedback control adjusts the amount of the output that is fed back into the input
//              delay lines. A larger feedback values leads to a longer reverb tail. The delay time control adjusts the 'size' of
//              the room by adjusting all of the delay lengths (dominant echos) simultaneously

class jh_reverb6
{
    public:
        jh_reverb6(float *apf1_buf, float *apf2_buf, float *apf3_buf, float *apf6_buf, float *apf7_buf, float *apf8_buf, int apf_buf_size,
                    float *del1_buf, float *del2_buf, float *del3_buf, float *del4_buf, float *del5_buf, float *del6_buf, float *del7_buf, float *del8_buf, 
                    int del_buf_size);
        void reverb6_preoperation(float delay_pot, float mix_pot, float fdbk_pot);
        void reverb6_operation(float input, float *temp_out);        // temp_out = 2 item array
    private:
        apf_zm_rev apf1, apf2, apf3, apf6, apf7, apf8;
        fractional_delay2 del1, del2, del3, del4, del5, del6, del7, del8;
        float delay_time, mix, fdbk;
        float HA_out1, HA_out2, HA_out3, HA_out4;
        float HB_out1, HB_out2, HB_out3, HB_out4;
};

#endif