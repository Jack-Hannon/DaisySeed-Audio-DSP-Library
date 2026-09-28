
#include "jh_schroeder_apf.h"

// Description: Mono phaser class with tunable gain (mapped to LFO in stereo class), mix, and feedback
//              Program uses 16 schroeder all pass filters tuned to about the same gain to accumulate phase shifts without influencing frequency spectrum
//              By mixing the input (not phase shifted) signal with the phaser output, destructive interference occurs at frequencies where the phase shift
//              is 180 degrees * (2n + 1) and constructive interference where the phase shift is 180 * 2n. APF gain parameter alters position of the troughs
//              in the frequency domain
//              Feedback increases the 'strength' of the effect, and contributes resonant artifacts when feedback is high


class JH_phaser_v2
{
    public:
        JH_phaser_v2(): apf1(), apf2(), apf3(), apf4(),
                        apf5(), apf6(), apf7(), apf8(),
                        apf9(), apf10(), apf11(), apf12(),
                        apf13(), apf14(), apf15(), apf16()
        {
            gain = 0.0f;
            mix = 0.0f;   
            phaser_out = 0.0f;   
            fdbk = 0.0f;
        }
        inline void phaser_v2_preoperation(float gain_pot, float mix_pot, float fdbk_pot)
        {
            // slew gain, mix, and feedback controls
            gain = gain * 0.95 + 0.05 * (0.2 + 0.7 * gain_pot);
            mix = mix * 0.95 + 0.05 * mix_pot;
            fdbk = fdbk * 0.95 + 0.05 * fdbk_pot;

            apf1.apf_preoperation(gain);
            apf2.apf_preoperation(gain);
            apf3.apf_preoperation(gain);
            apf4.apf_preoperation(gain);
            apf5.apf_preoperation(gain);
            apf6.apf_preoperation(gain);
            apf7.apf_preoperation(gain);
            apf8.apf_preoperation(gain);
            apf9.apf_preoperation(gain);
            apf10.apf_preoperation(gain);
            apf11.apf_preoperation(gain);
            apf12.apf_preoperation(gain);
            apf13.apf_preoperation(gain);
            apf14.apf_preoperation(gain);
            apf15.apf_preoperation(gain);
            apf16.apf_preoperation(gain);
        }

        inline float phaser_v2_operation(float input)
        {
            // mix input with attenuated phaser output and process signal through serial APF chain
            float phaser_input = input + fdbk * phaser_out;
            float apf1_out = apf1.apf_operation(phaser_input);
            float apf2_out = apf2.apf_operation(apf1_out);
            float apf3_out = apf3.apf_operation(apf2_out);
            float apf4_out = apf4.apf_operation(apf3_out);
            float apf5_out = apf5.apf_operation(apf4_out);
            float apf6_out = apf6.apf_operation(apf5_out);
            float apf7_out = apf7.apf_operation(apf6_out);
            float apf8_out = apf8.apf_operation(apf7_out);
            float apf9_out = apf9.apf_operation(apf8_out);
            float apf10_out = apf10.apf_operation(apf9_out);
            float apf11_out = apf11.apf_operation(apf10_out);
            float apf12_out = apf12.apf_operation(apf11_out);
            float apf13_out = apf13.apf_operation(apf12_out);
            float apf14_out = apf14.apf_operation(apf13_out);
            float apf15_out = apf15.apf_operation(apf14_out);
            phaser_out = apf16.apf_operation(apf15_out);

            float mix_output = (phaser_out * mix + input * (1 - mix));

            return mix_output;
        }

    private:
        float gain, mix, phaser_out, fdbk;
        schroeder_apf_v2 apf1, apf2, apf3, apf4,
                        apf5, apf6, apf7,apf8,
                        apf9, apf10, apf11, apf12,
                        apf13, apf14, apf15, apf16;
};