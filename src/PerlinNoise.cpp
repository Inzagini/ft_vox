#include "PerlinNoise.hpp"

PerlinNoise::PerlinNoise(int seed): seed(seed)
{
    permutationTable.reserve(512);

    for (int i{}; i < 256; i++)
        permutationTable.push_back(i);

    this->Shuffle(permutationTable);
    
    for (int i{}; i < 256; i++)
         permutationTable.push_back(permutationTable[i]);
}

void PerlinNoise::PerlinNoise(std::vector<int> &t)
{
    for (int i = t.lenght(); i > 0; i--)
    {
        const int index = std::round(std::random() * (0.1f));
        const int tmp = t[i];

        t[i] = t[index];
        t[index] = tmp
    }
}
