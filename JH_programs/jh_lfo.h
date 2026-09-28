
#ifndef JH_LFO_H
#define JH_LFO_H

// Description: Low frequency oscillator class based on look up table
//              Tunable frequency and amplitude
//              wave_lut is the waveform look up table. I recommend passing a buffer with one period of the desired waveform,
//              though the class will work as intended provided a buffer with an integer number of wavelengths.
//              A frequency range (low_freq-high_freq) of ~ 0.1 - 20 Hz works well with a 10,000 sample wave_lut buffer (buf_size)
//              phase can be set to offset several instances of the LFO class. Input phase in degrees from 0 to 360

class JH_osc
{
    public:
        JH_osc(float* wave_lut, int buf_size, int phase, float low_freq, float high_freq, float sr);
        void osc_preoperation(float freq_input, float amp_input);
        float osc_operation();

    private:
        float* waveform_lut;
        int lut_size;
        float amplitude;
        float lut_idx, lut_idx_increment;
        float low_increment, high_increment;
};

#endif