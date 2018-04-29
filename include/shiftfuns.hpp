// Defines the AffineShift class, which is used for advancing the seed.
//
// An AffineShift represents a map of the form x |--> (a * x + b) mod (2 ^ 32).
// These maps form a monoid, so it is possible to advance the seed n steps in
// O(log n) time by using exponentiation by squaring.
//
// The seed is represented by a uint32_t because we want defined overflow
// mod (2 ^ 32).

#ifndef SHIFTFUNS_HPP
#define SHIFTFUNS_HPP

#include <cstdint>

class AffineShift {
public:
    AffineShift(uint32_t m, uint32_t i)
        : mult{m}, inc{i} {}
    uint32_t multiplier() const { return mult; }
    uint32_t increment() const { return inc; }
    uint32_t operator()(uint32_t seed) const;
    AffineShift& operator*=(const AffineShift& rhs);
    AffineShift& operator^=(long long exponent);
private:
    uint32_t mult;
    uint32_t inc;
    AffineShift& square();
    AffineShift& pow(uint32_t exponent);
};

AffineShift operator*(AffineShift shift1, const AffineShift& shift2);
AffineShift operator^(AffineShift shift, long long exponent);

AffineShift fmShift();

#endif
