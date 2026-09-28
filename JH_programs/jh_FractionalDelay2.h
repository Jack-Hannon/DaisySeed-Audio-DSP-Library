
#include "jh_utility.h"

#ifndef FRACTIONALDELAY2_H
#define FRACTIONALDELAY2_H

// Description: Lighter fractional delay class without a feedback path filter. Used in the reverb6 and flanger effects
//              See jh_delay3.h for a discussion on tuning fixed parameters

class fractional_delay2
{
public:
    fractional_delay2(float *delay_buf, int buffer_size);
    void delay_preoperation(float delay_pot, float mix_pot, float fdbk_pot);
    float delay_operation(float input);

private:
    float delT, mix, fdbk;
    float *delay_buffer;
    int write_idx;
    int buf_size;
    float output;
};

#endif



