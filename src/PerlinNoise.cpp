#include "PerlinNoise.hpp"
#include <algorithm>
#include <iostream>
#include <random>

PerlinNoise::PerlinNoise(int seed) : seed(seed) {
    permutationTable.resize(256);

    std::iota(permutationTable.begin(), permutationTable.end(), 0);

    std::default_random_engine engine(seed);
    std::shuffle(permutationTable.begin(), permutationTable.end(), engine);

    permutationTable.insert(permutationTable.end(), permutationTable.begin(),
                            permutationTable.end());
}

double PerlinNoise::grad(const int hash, const double x, const double y) {
    int h = hash & 7;
    double u = h < 4 ? x : y;
    double v = h < 4 ? y : x;

    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0 * v : 2.0 * v);
}

double PerlinNoise::noise(const double x, const double y) {
    const int X = (int)std::floor(x) & 255;
    const int Y = (int)std::floor(y) & 255;

    double xf = x - std::floor(x);
    double yf = y - std::floor(y);

    double u = fade(xf);
    double v = fade(yf);

    int A = permutationTable[X] + Y;
    int B = permutationTable[X + 1] + Y;

    int aa = permutationTable[A];
    int ab = permutationTable[A + 1];
    int ba = permutationTable[B];
    int bb = permutationTable[B + 1];

    double x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);

    double x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);

    return lerp(x1, x2, v);
}

double PerlinNoise::octaveNoise(const double x, const double y, const int octaves,
                                const double persistence, const double lacunarity) {
    double amplitude{1.0f};
    double frequency{1.0f};
    double total{};
    double maxValue{};

    for (int i{}; i < octaves; i++) {
        total += noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return total / maxValue;
}
