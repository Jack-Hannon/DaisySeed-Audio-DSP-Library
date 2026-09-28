
#ifndef JH_APFZ1_H
#define JH_APFZ1_H

// Description: Schroeder all pass filter fixed to unit delay.
//              Tunable feedforward/feedback gain parameter

class apf_z1
{
    public:
        apf_z1();
        void apf_preoperation(float gain_pot);
        float apf_operation(float input);
    private:
        float gain, out, z1, z0;
};

#endif