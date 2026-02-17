#pragma once

#include "glm/glm.hpp"
#include <cmath>

class  PerlinNoise
{
    public:
        PerlinNoise(int seed);

    private:
        void Shuffle(std::vector<int> &);

    private:
        int seed;
        std::vector<int> permutationTable;

};

