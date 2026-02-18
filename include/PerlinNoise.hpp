#pragma once

#include <random>
#include <cmath>

class  PerlinNoise
{
    public:
        PerlinNoise(const int seed);

    private:
        void Shuffle(std::vector<int> &);
        double fade(const double t) const
        { return t * t * t * (t * ( t * 6 - 15) + 10); }

        double lerp(const double a, const double b, const double x)
        { return a + x * (b - a); }

        double grad(const int hash,const double x, const double y);
        double noise(const double x, const double y);
        double octaveNoise(const double, const double, const int, const double, const double);

    private:
        int seed;
        std::vector<int> permutationTable;

};
    
