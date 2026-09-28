
#include "jh_svf.h"


// Description: State variable filter based on https://kokkinizita.linuxaudio.org/papers/digsvfilt.pdf
//              Referene this paper for insight into intermediate variables. 
//              Provides control over cutoff frequency from 80 Hz to 8 kHz, resonance (Q) from 0.6 to 7.6,
//              and various filter outputs (low pass, high pass, and band pass)
//              Must pass a tangent() look up table to correct for frequency warping effects.


svf_v2::svf_v2(float tangent_lut[10000])
{
    // initialize filter states
    z1_out = 0;
    z2_out = 0;
    A1_out = 0;

    fc_val = 0;
    Q_val = 0;
    type_pot = 0.0f;

    tan_LUT = tangent_lut;
    LUT_idx = 0;

    output = 0;
}

void svf_v2::svf_v2_preoperation(float freq_pot, float resonance_pot, float filter_type_pot)        //pot inputs should be floats limited to range 0 to 1
{
    // parabolic cutoff frequency mapping
    fc_val = fc_val * 0.95 + 0.05 * (76 + 8090 * freq_pot * freq_pot);

    // Smoothed resonance control
    Q_val = Q_val * 0.95 + 0.05 * (0.6 + 7 * resonance_pot);

    // 0-0.33 LPF, 0.34-0.66 BPF, 0.67-1 HPF
    type_pot = type_pot * 0.95 + 0.05 * filter_type_pot;

    // frequency warping
    float pi_F = (3.14159265 * fc_val) / 48000.0f;
    LUT_idx = (int)(pi_F * 10000.0f);
    float fraction = (pi_F * 10000.0f) - LUT_idx;
    w = 2 * (tan_LUT[LUT_idx]*(1-fraction) + (fraction) * tan_LUT[LUT_idx + 1]);

    a = w/(Q_val);
    b = w*w;

    c1 = (a + b) / (1 + a/2 + b/4);
    c2 = b / (a+b);

    // LPF, HPF, & BPF can be switched using a pot. For pot values from 0 to 0.33 LPF should be output, for 0.34-0.66, BPF selected, for 0.67-1, HPF output
    if (type_pot <= 0.33f){                             //LPF
        d0 = c1 * c2/4;
        d1 = c2;
        d2 = 1;
    }
    else if (type_pot > 0.33f && type_pot <= 0.66f){    //BPF
        d0 = (1-c2) * (c1/2);
        d1 = 1-c2;
        d2 = 0;
    }
    else{                                               //HPF
        d0 = 1 - c1/2 + c1*c2/4;;
        d1 = 0;
        d2 = 0;
    }
}

float svf_v2::svf_v2_operation(float input)
{
    A1_out = input - z1_out - z2_out;
    output = d0 * A1_out + d1 * z1_out + d2 * z2_out;
    z2_out += z1_out * c2;
    z1_out += A1_out * c1;
    return output;
}
