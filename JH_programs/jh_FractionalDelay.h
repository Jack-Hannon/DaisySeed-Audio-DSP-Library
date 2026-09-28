
#include "jh_svf.h"

#ifndef FRACTIONALDELAY_H
#define FRACTIONALDELAY_H

// Description: Early fractional delay effect using linear interpolation and a feedback filter.
//              See jh_delay3.h for a discussion on tuning fixed parameters.

class fractional_delay
{
public:
    fractional_delay(float *delay_buf, int buffer_size, bool modulation, float tangent[10000]);
    void delay_preoperation(float pot0, float pot1, float pot2);
    float delay_operation(float input);

private:
    float delT, mix, fdbk;
    float *delay_buffer;
    bool mod_switch;
    int write_idx;
    int buf_size;
    float output;
    svf_v2 svf1;
};

#endif



