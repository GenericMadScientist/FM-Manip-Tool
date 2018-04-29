#include <stdexcept>

#include "constants.hpp"
#include "seedfuns.hpp"
#include "shiftfuns.hpp"

uint32_t intPow(uint32_t base, uint32_t exp)
{
    if (exp == 0) {
        return 1;
    }
    else if (exp % 2 == 0) {
        return intPow(base * base, exp / 2);
    }
    else {
        return base * intPow(base * base, exp / 2);
    }
}

uint32_t advance(uint32_t seed, long long advances)
{
    return (fmShift() ^ advances)(seed);
}

uint32_t startSeed()
{
    return 0x55555555; // initial FM seed
}

uint32_t indexToSeed(long long index)
{
    return advance(startSeed(), index);
}

// Solves the discrete logarithm problem for powers of the ANSI C LCG
// multiplier, modulo 2^32. Raises an exception if the "power" is not 1 mod 4.
uint32_t oddDiscreteLog(uint32_t powerOfMult)
{
    if (powerOfMult % 4 != 1) {
        throw std::invalid_argument("oddDiscreteLog argument not a "
                                    "power of 1103515245.");
    }

    uint32_t ansiMult = fmShift().multiplier();
    uint32_t twoPower = 1;
    uint32_t exponent = 0;

    for (int power = 1; power < 32; ++power) {
        twoPower *= 2;
        if (intPow(ansiMult, exponent) % twoPower != powerOfMult % twoPower) {
            exponent += twoPower / 8;
        }
    }
    // This conditional is needed for mod 2^32 check.
    if (intPow(ansiMult, exponent) != powerOfMult) {
        exponent += twoPower / 4;
    }

    return exponent;
}

uint32_t distFromZero(uint32_t seed)
{
    constexpr uint32_t shiftInv = 1440005641;
    uint32_t powerPeriod = intPow(2, 30);

    uint32_t powOfMult = 1 + shiftInv * (rngMult - 1) * seed;

    uint32_t possibleExp = oddDiscreteLog(powOfMult);

    while (advance(0, possibleExp) != seed) {
        possibleExp += powerPeriod;
    }

    return possibleExp;
}

// This procedure relies on a nice formula for advancing the seed n times.
// If we write f for the advance function x |--> (a * x + b) mod m, then
// a simple proof by induction shows that
//
// (f ^ n)(x) = (a ^ n) * x + b * (1 + a + ... + a ^ (n - 1)) mod m.
//
// In the case that concerns us b and m are coprime, so b has a multiplicative
// inverse. By letting x = 0, dividing by b, and multiplying by (a - 1) we get
//
// (a - 1) * (b ^ (-1)) * (f ^ n)(0) = ((a ^ n) - 1) mod m.
//
// We can therefore compute n modulo 2 ^ 30 by solving the discrete logarithm
// problem, which for m = 2 ^ k we can do in O(k). This gives us four
// possibilities, which we can check in O(k) time.
long long seedToIndex(uint32_t seed)
{
    uint32_t initIndex = distFromZero(startSeed());
    return distFromZero(seed) - initIndex;
}

uint32_t prn(uint32_t seed)
{
    return (seed >> 16) & 0x7FFF;
}
