#include <iostream>
#include <cmath>

// wavetypes

struct
{
    double sinusoid(double s_index_val, int sample_rate)
    {
        return sin(s_index_val / sample_rate);
    }

    double square(double s_index_val, int sample_rate)
    {
        if (sin(s_index_val / sample_rate) > 0)
            return 1;
        else
            return -1;
    }
    double saw(int frequency, int)
} BasicWaves;

constexpr unsigned int str2int(const char *str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}