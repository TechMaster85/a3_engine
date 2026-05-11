#pragma once

#include <random>

class RandomEngine {
    std::default_random_engine engine;
    std::uniform_real_distribution<float> distribution;

public:
    /* Please look up the correct seed value for your usage purposes. Find it in
     * the homework9 assignment spec. */
    RandomEngine(float min, float max, int seed) { Configure(min, max, seed); }

    /* Please look up the correct seed value for your usage purposes. Find it in
     * the homework9 assignment spec. */
    void Configure(float min, float max, int seed) {
        engine = std::default_random_engine(static_cast<uint>(seed));
        distribution = std::uniform_real_distribution<float>(min, max);
    }

    /* If you use this constructor, don't forget to configure the engine later.
     */
    RandomEngine() = default;

    /* Get a random number in the specified range. */
    float Sample() { return distribution(engine); }
};
