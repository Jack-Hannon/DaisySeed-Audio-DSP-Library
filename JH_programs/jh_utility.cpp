
#include "jh_utility.h"

// Description: Useful functions for interpolation and clipping.


float linear_interpolation(float prior_value, float next_value, float fraction)
{
    return prior_value * (1-fraction) + next_value * fraction;
}

// f(x) = g * in / (1 + g * |x|)
float soft_clip(float in, float saturation)
{
    float abs_in = (in < 0) ? (in * -1) : in;
    return ( (saturation * in) / (1 + saturation * abs_in));
}

// f(x) = 2 * g * in / (1 + g * |x|)
float soft_clip2(float in, float saturation)
{
    float abs_in = (in < 0) ? (in * -1) : in;
    return ( (2 * saturation * in) / (1 + saturation * abs_in));
}

// f(x) = g * in / (1 + 0.3 * g * |x|)
float soft_clip3(float in, float saturation)
{
    float abs_in = (in < 0) ? (in * -1) : in;
    return ( (saturation * in) / (1 + saturation * 0.3 * abs_in));
}

// increment is current idx_inc value, dist is distance from 2nd float write idx to current idx for interpolation
// oldest sample: .... y0  y1  y2  y3  ... newest sample        m0 = slope at y1
float hermite_interpolation1(float y0, float y1, float y2, float y3, int index1, float target_idx)
{
    float m0 = 0.5f * (y2 - y0);        // slope at y1
    float m1 = 0.5f * (y3 - y1);        // slope at y2

    float t = target_idx - index1;

    float h00 = 2*t*t*t - 3*t*t + 1;
    float h10 = t*t*t - 2*t*t + t;
    float h01 = -2*t*t*t + 3*t*t;
    float h11 = t*t*t - t*t;

    return h00*y1 + h10*m0 + h01*y2 + h11*m1;
}