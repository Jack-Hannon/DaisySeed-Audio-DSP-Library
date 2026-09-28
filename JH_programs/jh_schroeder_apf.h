
#ifndef SCHROEDER_APF_V2_H
#define SCHROEDER_APF_V2_H

// Description: Schroeder APF class supporting phaser program

class schroeder_apf_v2
{
    public:
        schroeder_apf_v2();
        inline void apf_preoperation(float gain_pot);
        inline float apf_operation(float input);

    private:
        float gain, out, z1, z0;
};

#endif
