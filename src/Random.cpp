#include <RaeptorCogs/Random.hpp>
#include <cstdlib>

int RaeptorCogs::Singletons::Random::getInt(int min, int max) {
    return min + (std::rand() % (max - min + 1));
}

float RaeptorCogs::Singletons::Random::getFloat(float min, float max) {
    float scale = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return min + scale * (max - min);
}