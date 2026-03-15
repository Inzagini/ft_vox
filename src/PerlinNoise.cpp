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
    const int h = hash & 7;
    const double u = h < 4 ? x : y;
    const double v = h < 4 ? y : x;

    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0 * v : 2.0 * v);
}

double PerlinNoise::noise(const double x, const double y) {

    const int xi = (int)std::floor(x);
    const int yi = (int)std::floor(y);

    const int X = xi & 255;
    const int Y = yi & 255;

    const double xf = x - xi;
    const double yf = y - yi;

    const double u = fade(xf);
    const double v = fade(yf);

    const int A = permutationTable[X] + Y;
    const int B = permutationTable[X + 1] + Y;

    const int aa = permutationTable[A];
    const int ab = permutationTable[A + 1];
    const int ba = permutationTable[B];
    const int bb = permutationTable[B + 1];

    const double x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
    const double x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);

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
