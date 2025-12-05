#include <RaeptorCogs/BitOp.hpp>
namespace RaeptorCogs {

uint64_t NextPowerOf2(uint64_t n) {
    if (n == 0) return 0; // Handle the case for 0
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32; // For 64-bit integers
    return n + 1;
}

}