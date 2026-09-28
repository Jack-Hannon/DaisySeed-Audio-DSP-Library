
#include "jh_apf_z1.h"

// Schroeder All Pass Filter

apf_z1::apf_z1()
{
    gain = 0.0f;
    out = 0.0f;
    z1 = 0.0f;
    z0 = 0.0f;
}

void apf_z1::apf_preoperation(float gain_pot)
{
    gain = gain_pot;
}

float apf_z1::apf_operation(float input)
{
    out = z1 - gain * z0;
    z1 = z0;
    z0 = input + gain * z1;
    return out;
}


