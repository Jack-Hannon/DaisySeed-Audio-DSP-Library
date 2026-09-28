
#ifndef JHUTILITY_H
#define JHUTILITY_H

// Description: Useful functions for interpolation and clipping.

// approximate the value at a fractional index (fraction = fractional_idx - previous_integer_idx)
float linear_interpolation(float prior_value, float next_value, float fraction);

// Variations on soft clipping function: f(x) = g * x / (1 + g * |x|)
float soft_clip(float in, float saturation);
float soft_clip2(float in, float saturation);
float soft_clip3(float in, float saturation);

float hermite_interpolation1(float y0, float y1, float y2, float y3, int index1, float target_idx);

#endif



