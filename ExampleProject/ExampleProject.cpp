
#include "daisy_seed.h"
#include "daisysp.h"

#include "JH_programs\jh_FractionalDelay.h"
#include "JH_programs\jh_FractionalDelay2.h"
#include "JH_programs\jh_utility.h"         
#include "JH_programs\jh_reverb4.h"       
#include "JH_programs\jh_svf.h"
#include "JH_programs\jh_lfo.h"
#include "JH_programs\jh_reverb6.h"
#include "JH_programs\jh_flanger.h"
#include "JH_programs\jh_stereo_phaser16.h"
#include "JH_programs\jh_delay3.h"
#include "JH_programs\jh_stereo_delay.h"
#include "JH_programs\JH_freq_shift1.h"
#include "JH_programs\jh_freq_shift2.h"

#include <cmath>

using namespace daisy;


DaisySeed hw;

float DSY_SDRAM_BSS tang_LUT[10000];                    // 1. Filter
svf_v2 svf1(tang_LUT);

const int delay_size = 4 * 48000;                       // 2. Delay
float DSY_SDRAM_BSS delay_bufferL[delay_size];
float DSY_SDRAM_BSS delay_bufferR[delay_size];
stereo_delay delay(delay_bufferL, delay_bufferR, delay_size, tang_LUT);

float DSY_SDRAM_BSS lfo_lut[10000];             // Sine LUT for modulation
const int lfo_lut_size = 10000;

stereo_phaser16 phaser(lfo_lut, lfo_lut_size, 0.02f, -0.01f, -0.03f, 0.02f, 0, 180);        // 3. Phaser

float DSY_SDRAM_BSS delR_buf[400];                      // 4. Flanger
float DSY_SDRAM_BSS delL_buf[400];
const int del_buf_size = 400;
jh_flanger flanger(delR_buf, delL_buf, del_buf_size, lfo_lut, lfo_lut_size);        

float DSY_SDRAM_BSS d1_buf[9600];                       // 5. Reverb
float DSY_SDRAM_BSS d2_buf[9600];
float DSY_SDRAM_BSS d3_buf[9600];
float DSY_SDRAM_BSS d4_buf[9600];
float DSY_SDRAM_BSS d5_buf[9600];            
float DSY_SDRAM_BSS d6_buf[9600];
float DSY_SDRAM_BSS d7_buf[9600];
float DSY_SDRAM_BSS d8_buf[9600];

float DSY_SDRAM_BSS a1_buf[1000];            
float DSY_SDRAM_BSS a2_buf[1000];
float DSY_SDRAM_BSS a3_buf[1000];
float DSY_SDRAM_BSS a6_buf[1000];            
float DSY_SDRAM_BSS a7_buf[1000];
float DSY_SDRAM_BSS a8_buf[1000];

const int d_buf_size = 9600;
const int apf_buf_size = 1000;

jh_reverb6 reverb(a1_buf, a2_buf, a3_buf, a6_buf, a7_buf, a8_buf, apf_buf_size,
    d1_buf, d2_buf, d3_buf, d4_buf, d5_buf, d6_buf, d7_buf, d8_buf, d_buf_size);


float DSY_SDRAM_BSS sin_lut[5000];                  // 6. Frequency Shifter
float DSY_SDRAM_BSS cos_lut[5000];
int quad_size = 5000;
float audio_buf[501];       // keep on chip for fast access during long convolution loop
float input_buf[251];
jh_freq_shifter2 FS1(sin_lut, cos_lut, quad_size, audio_buf, input_buf, 500.0f, 48000, tang_LUT);

const float pi = 3.14159;
int current_effect = 0;
const float num_effects = 6.99f;
float pot0 = 0.0f, pot1 = 0.0f, pot2 = 0.0f, pot_switch = 0.0f;
float temp_out[2] = {0.0f, 0.0f};

void MyCallback(AudioHandle::InputBuffer in,
                AudioHandle::OutputBuffer out,
                size_t size)
{
        pot0 = hw.adc.GetFloat(0);
        pot1 = hw.adc.GetFloat(1);
        pot2 = hw.adc.GetFloat(2);
        pot_switch = pot_switch * 0.95 + 0.05 * hw.adc.GetFloat(3);

    // Potentiometer value (0 - 1) is quantized to an integer to select an effect
    int effect_select = static_cast<int>((pot_switch * num_effects));

    if (effect_select >= num_effects) effect_select = num_effects - 1;
    float temp_output_mono = 0.0f;

    switch(effect_select)
    {
        case 0:     // Bypass
            for(size_t i = 0; i < size; i++)
            {
                out[0][i] = in[0][i];
                out[1][i] = in[0][i];
            }
            break;

        case 1:     // Filter
            svf1.svf_v2_preoperation(pot0, pot1, pot2);
            for (size_t i = 0; i < size; i++)
            {
                temp_output_mono = svf1.svf_v2_operation(in[0][i]);
                out[0][i] = temp_output_mono;
                out[1][i] = temp_output_mono;
            }
            break;

        case 2:     // Delay/Echo
            delay.stereo_delay_preoperation(pot0, pot1, pot2);
            for (size_t i = 0; i < size; i++)
            {
                delay.stereo_delay_operation(in[0][i], temp_out);
                out[0][i] = temp_out[0];
                out[1][i] = temp_out[1];
            }
            break;

        case 3:     // Phaser
            phaser.stereo_phaser16_preoperation(pot0, pot1, pot2);
            for (size_t i = 0; i < size; i++)
            {
                phaser.stereo_phaser16_operation(in[0][i], temp_out);
                out[0][i] = temp_out[0];
                out[1][i] = temp_out[1];
            }
            break;

        case 4:     // Flanger
            flanger.flanger_preoperation(pot0, pot1, pot2);
            for (size_t i = 0; i < size; i++)
            {
                flanger.flanger_operation(in[0][i], temp_out);
                out[0][i] = temp_out[0];
                out[1][i] = temp_out[1];
            }
            break;

        case 5:     // Reverb 
            reverb.reverb6_preoperation(pot0, pot1, pot2);
            for (size_t i = 0; i < size; i++)
            {
                reverb.reverb6_operation(in[0][i], temp_out);
                out[0][i] = temp_out[0];
                out[1][i] = temp_out[1];
            }
            break;

        case 6:     // Frequency shfiter
            FS1.preoperation(pot0, pot2);
            for (size_t i = 0; i < size; i++)
            {
                float temp_out = FS1.operation(in[0][i]);
                out[0][i] = temp_out;
                out[1][i] = temp_out;
            }

    }
}

int main(void)
{
    hw.Init();

    for (int idx = 0; idx < 10000; idx++)
    {
        float i = idx * 0.0001f;
        tang_LUT[idx] = tan(i);
    }

    for (int i = 0; i < delay_size; i++)        // initialize delay buffers
    {
        delay_bufferL[i] = 0.0f;
        delay_bufferR[i] = 0.0f;
    }

    for (int k = 0; k < del_buf_size; k++)      // initialize flanger delay buffers
    {
        delR_buf[k] = 0.0f;
        delL_buf[k] = 0.0f;
    }

    for (int j = 0; j < lfo_lut_size; j++)      // store sine wave in LFO buffer
    {
        lfo_lut[j] = sin( (2 * pi * j) / (lfo_lut_size) );  
    }

    for (int j = 0; j < d_buf_size; j++)        // initialize reverb buffers
    {
        d1_buf[j] = 0.0f;
        d2_buf[j] = 0.0f;
        d3_buf[j] = 0.0f;
        d4_buf[j] = 0.0f;
        d5_buf[j] = 0.0f;
        d6_buf[j] = 0.0f;
        d7_buf[j] = 0.0f;
        d8_buf[j] = 0.0f;
        if (j < apf_buf_size)
        {
            a1_buf[j] = 0.0f;
            a2_buf[j] = 0.0f;
            a3_buf[j] = 0.0f;
            a6_buf[j] = 0.0f;
            a7_buf[j] = 0.0f;
            a8_buf[j] = 0.0f;
        }
    }

    for (int i = 0; i < quad_size; i++)                     // Initialize quadrature oscillator
    {
        sin_lut[i] = sin((2 * pi * i) / (quad_size));
        cos_lut[i] = cos((2 * pi * i) / (quad_size));
        if(i < 501) audio_buf[i] = 0.0f;
        if (i < 251) input_buf[i] = 0.0f;
    }

    // Configure potentiometers
    AdcChannelConfig adc_cfg[4];
    adc_cfg[0].InitSingle(daisy::seed::A0); 
    adc_cfg[1].InitSingle(daisy::seed::A1); 
    adc_cfg[2].InitSingle(daisy::seed::A2); 
    adc_cfg[3].InitSingle(daisy::seed::A3); 

    hw.adc.Init(adc_cfg, 4);

    hw.adc.Start();

    hw.StartAudio(MyCallback);

    while(1)
    {
    }
}
