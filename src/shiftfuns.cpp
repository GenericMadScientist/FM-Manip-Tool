#include <cstdint>

#include "constants.hpp"
#include "shiftfuns.hpp"

AffineShift& AffineShift::square()
{
    inc *= mult + 1;
    mult *= mult;
    return *this;
}

AffineShift& AffineShift::pow(uint32_t exponent)
{
    if (exponent == 0) {
        mult = 1;
        inc = 0;
        return *this;
    }
    else if (exponent % 2 == 0) {
        square();
        return pow(exponent / 2);
    }
    else {
        AffineShift tempShift = *this;
        square();
        pow(exponent / 2);
        *this *= tempShift;
        return *this;
    }
}

uint32_t AffineShift::operator()(uint32_t seed) const
{
    return mult * seed + inc;
}

AffineShift& AffineShift::operator*=(const AffineShift& rhs)
{
    inc += mult * rhs.inc;
    mult *= rhs.mult;
    return *this;
}

AffineShift& AffineShift::operator^=(long long exponent)
{
    constexpr long long maxSeedSize = 4294967296; // 2 ^ 32
    exponent = ((exponent % maxSeedSize) + maxSeedSize) % maxSeedSize;
    pow(exponent);
    return *this;
}

AffineShift operator*(AffineShift shift1, const AffineShift& shift2)
{
    shift1 *= shift2;
    return shift1;
}

AffineShift operator^(AffineShift shift, long long exponent)
{
    shift ^= exponent;
    return shift;
}

AffineShift fmShift()
{
    return AffineShift(rngMult, rngInc); // ANSI C LCG used by FM
}
