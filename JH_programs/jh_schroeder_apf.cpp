

// Description: Schroeder APF class supporting phaser program

class schroeder_apf_v2
{
    public:
        schroeder_apf_v2()
        {
            gain = 0.0f;
            out = 0.0f;
            z1 = 0.0f;
            z0 = 0.0f;
        }
        inline void apf_preoperation(float gain_pot)
        {
            gain = gain_pot;
        }
        inline float apf_operation(float input)
        {
            out = z1 - gain * z0;
            z1 = z0;
            z0 = input + gain * z1;
            return out;
        }
    private:
        float gain, out, z1, z0;
};
