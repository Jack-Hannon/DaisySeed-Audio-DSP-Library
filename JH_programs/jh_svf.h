
#ifndef SVF_V2_H
#define SVF_V2_H

// Description: State variable filter based on https://kokkinizita.linuxaudio.org/papers/digsvfilt.pdf
//              Referene this paper for insight into intermediate variables. 
//              Provides control over cutoff frequency from 80 Hz to 8 kHz, resonance (Q) from 0.6 to 7.6,
//              and various filter outputs (low pass, high pass, and band pass)
//              Must pass a tangent() look up table to correct for frequency warping effects.

class svf_v2
{
    public:
        // tangent look-up-table required to correct frequency warping
        svf_v2(float tangent_lut[10000]);

        // Update algorithm parameters on per-block basis
        void svf_v2_preoperation(float freq_pot, float resonance_pot, float filter_type_pot);

        // per-sample processing
        float svf_v2_operation(float input);
        
    private:
        int LUT_idx;
        float w, a, b, c1, c2;
        float d0, d1, d2;                   // filter coefficients
        float z1_out, z2_out, A1_out;       // filter states
        float fc_val, Q_val, type_pot;
        float* tan_LUT; 
        float output;    
};

#endif