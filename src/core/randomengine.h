#pragma once

#include <cstdint>
#include <random>

class RandomEngine {
    std::default_random_engine engine;
    std::uniform_real_distribution<float> distribution;

public:
    RandomEngine(float min, float max, int seed) { Configure(min, max, seed); }

    void Configure(float min, float max, int seed) {
        engine = std::default_random_engine(static_cast<uint32_t>(seed));
        distribution = std::uniform_real_distribution<float>(min, max);
    }

    RandomEngine() = default;

    float Sample() { return distribution(engine); }
};
