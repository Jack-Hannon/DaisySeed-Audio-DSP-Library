
#include "jh_freq_shift2.h"

// Description: Hilbert transform-based frequency shifter with a tunable frequency shift amount and sideband suppression control.
//              Implementation uses a fixed 501-tap Hilbert FIR.
//              Frequency shift range spans from -TOP_FREQ to +TOP_FREQ in Hz
//              Sideband mix parameter controls the relative weight of the sidebands (0 = DSB-SC, 1 = SSB-SC)


float hilbert_fir(float audio_buf[501], float input, int &write_idx)
{

    // The hilbert transform may be implemented as a type 3 FIR filter in matlab using firpm()
    // In this case, the hilbert transform is antisymmetric about the (N-1)/2 tap and all of the even taps are zero
    // The odd FIR taps for the first half of the filter are listed below. All zero valued taps are omitted.
    static const float h_n[] = {
        0.00392047f, 0.00039516f, 0.00041465f, 0.0004346f,  0.00045512f, 0.00047632f, 0.00049843f, 0.00052153f, 0.00054565f, 0.00057062f,
        0.00059611f, 0.00062165f, 0.00064677f, 0.00067178f, 0.00069904f, 0.00073538f, 0.00075942f, 0.00079142f, 0.0008229f,  0.00085511f,
        0.00088798f, 0.00092158f, 0.000956f,   0.00099145f, 0.00102807f, 0.00106593f, 0.00110479f, 0.00114412f, 0.00118384f, 0.00122503f,
        0.00126941f, 0.00131155f, 0.00135698f, 0.00140309f, 0.00145055f, 0.00149923f, 0.00154912f, 0.00160015f, 0.0016524f,  0.00170602f,
        0.00176123f, 0.001818f,   0.001876f,   0.00193513f, 0.00199601f, 0.00205931f, 0.00212269f, 0.0021887f,  0.00225606f, 0.00232531f,
        0.00239644f, 0.00246954f, 0.00254453f, 0.00262149f, 0.00270049f, 0.00278178f, 0.00286546f, 0.00295133f, 0.00303941f, 0.00313022f,
        0.00322413f, 0.00331995f, 0.00341918f, 0.00352106f, 0.00362606f, 0.00373427f, 0.00384589f, 0.003961f,   0.00407975f, 0.00420226f,
        0.00432892f, 0.00445993f, 0.00459526f, 0.00473513f, 0.00488008f, 0.00503055f, 0.00518607f, 0.00534789f, 0.00551573f, 0.00569022f,
        0.00587172f, 0.00606079f, 0.00625783f, 0.00646344f, 0.00667809f, 0.00690271f, 0.00713801f, 0.00738464f, 0.00764346f, 0.00791573f,
        0.0082026f,  0.00850489f, 0.00882467f, 0.00916307f, 0.00952211f, 0.00990379f, 0.01031057f, 0.01074508f, 0.01121046f, 0.01171012f,
        0.01224853f, 0.01283055f, 0.0134618f,  0.01414915f, 0.01490094f, 0.01572699f, 0.01663904f, 0.01765231f, 0.01878474f, 0.02005954f,
        0.02150605f, 0.02316256f, 0.02507932f, 0.02732426f, 0.02999088f, 0.03321266f, 0.03718537f, 0.04220925f, 0.04876949f, 0.05770418f,
        0.07059615f, 0.09083714f, 0.12724648f, 0.2121601f,  0.6366043f};

    // circular buffer for FIR convolution
    audio_buf[write_idx] = input;

    float convolution_sum = 0.0f;

    // The newest input and oldest sample are both multiplied by zero during the convolution
    // target_idx walks backward in time from the newest sample to the sample at z^-249
    // pos_idx walks forward in time from the oldest sample to the sample at z^-251
    int target_idx = (write_idx - 1 >= 0) ? write_idx - 1 : 500;        // initialized to 2nd newest sample (first nonzero tap)
    int pos_idx = target_idx + 3;                                       // initialized to 2nd oldest sample - +3 indices from the 2nd newest in the circular buffer
    if (pos_idx >= 501) pos_idx -= 501;

    // Convolution block
    for (int i = 0; i < 125; i++)
    {
        // use antisymmetry to optimize convolution loop
        convolution_sum += h_n[i] * (audio_buf[pos_idx] - audio_buf[target_idx]);

        // since all even taps are zero, the target and pos indices take steps of 2
        target_idx -= 2;
        if (target_idx < 0) target_idx += 501;
        pos_idx += 2;
        if (pos_idx > 500) pos_idx -= 501;
    }
    
    write_idx = (write_idx + 1 < 501) ? write_idx + 1 : 0;

    return convolution_sum;
}


jh_freq_shifter2::jh_freq_shifter2(float* SIN_LUT, float* COS_LUT, int QUAD_SIZE, float hilbert_audio_buf[501], float input_audio_buf[251], float TOP_FREQ, float sample_rate, float tang_lut[10000]) : filter(tang_lut) 
{
    // quadrature oscillators
    sin_lut = SIN_LUT;
    cos_lut = COS_LUT;

    // Largest index jump that can happen when incrementing the sin and cos LUTs at the highest frequency.
    max_idx_step = (QUAD_SIZE * TOP_FREQ) / sample_rate;

    // quadrature oscillator buffer size
    quad_size = QUAD_SIZE;

    // I-Q input buffers
    input_buf = input_audio_buf;
    hilbert_buf = hilbert_audio_buf;

    // fractional oscillator LUT idx
    quad_read_idx = 0.0f;

    // through-zero frequency shift value
    bipolar_freq = 0.0f;  

    // sideband mix controlled via potentiometer: 0 = double side band modulation; 1 = single side band modulation
    SB_mix = 0.5;
    hilbert_write_idx = 0;
    input_write_idx = 0;
    freq = 0.5f;
}


void jh_freq_shifter2::preoperation(float freq_pot, float SB_mix_pot)
{
    // At 0.5, there is a frequency shift of 0 Hz
    // frequency control is manipulated so that positive and negative frequency shifts may be achieved
    freq = 0.99 * freq + 0.01 * freq_pot;       
    bipolar_freq = 2*(freq - 0.5);

    // cubic frequency shift control offers fine tuning at low frequencies and coarse tuning at high frequencies
    bipolar_freq = bipolar_freq * bipolar_freq * bipolar_freq;

    float SB_mix_adjusted = 0.5 + 0.5 * SB_mix_pot;
    SB_mix = 0.95 * SB_mix + 0.05 * SB_mix_adjusted;

    // ~80Hz cutoff, no resonance, HPF required to attenuate input frequency content outside of the
    // hilbert transform's effective range
    filter.svf_v2_preoperation(0.02, 0, 0.95);
}


float jh_freq_shifter2::operation(float input)
{
    // acquire in-phase signal delayed by hilbert FIR group delay (251 samples)
    float delayed_input = input_buf[input_write_idx];

    // filter input signal with HPF before passing to in-phase group delay buffer
    input_buf[input_write_idx] = filter.svf_v2_operation(input);

    input_write_idx = (input_write_idx + 1 < input_buf_size) ? input_write_idx + 1 : 0;

    // acquire quadrature signal processed through hilbert transfrom
    float hilbert_out = hilbert_fir(hilbert_buf, input, hilbert_write_idx);

    // update fractional quadrature oscillator read index based on the frequency shift input
    quad_read_idx += (max_idx_step * bipolar_freq);
    if (quad_read_idx >= quad_size) quad_read_idx -= quad_size;
    if (quad_read_idx < 0) quad_read_idx += quad_size;

    // compute oscillator value at fractional index using linear interpolation
    int idx1 = (int) quad_read_idx;
    int idx2 = (idx1 + 1 < quad_size) ? idx1 + 1 : 0;
    float sin_val = linear_interpolation(sin_lut[idx1], sin_lut[idx2], (quad_read_idx - idx1));
    float cos_val = linear_interpolation(cos_lut[idx1], cos_lut[idx2], (quad_read_idx - idx1));  

    // SSB calculations
    float USB = delayed_input * cos_val - hilbert_out * sin_val;        // upper side band
    float LSB = delayed_input * cos_val + hilbert_out * sin_val;        // lower side band
    return (SB_mix * USB + (1 - SB_mix) * LSB);
}



