#include <RaeptorLab/bit_op.hpp>

uint64_t next_power_of_2_64(uint64_t n) {
    if (n == 0) return 1; // Handle the case for 0
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32; // For 64-bit integers
    return n + 1;
}