
// SpaceGame.FixedPrecision

#ifndef SPACE_GAME_FIXED_PRECISION_H
#define SPACE_GAME_FIXED_PRECISION_H

#include <stdio.h>
#include <stdlib.h>

#include "WideInt.h"



// Store a fixed-precision decimal value as a pair of 64-bit unsigned
// integers, along with the sign.
//
typedef struct FixedPrec FixedPrec;

struct FixedPrec {
    int sign;
    uint64_t wholePart;
    uint64_t decPart;
};

// Are two fixed-precision values equal?
//
static inline int fpEqual (FixedPrec a, FixedPrec b) {
    return
        a.sign      == b.sign       &&
        a.wholePart == b.wholePart  &&
        a.decPart   == b.decPart;
}

// Compare two fixed-precision values.
//
static inline int fpLessThan (FixedPrec a, FixedPrec b) {
    if (fpEqual (a, b))
        return 1;
    if (a.sign < b.sign)
        return 1;
    else if (a.wholePart < b.wholePart)
        return 1;
    else if (a.decPart < b.decPart)
        return 1;
    else
        return 0;
}

// Compute the absolute value of a fixed-precision number.
//
// (Fancy way of saying set the sign field to 1).
//
static inline FixedPrec fpAbs (FixedPrec a) {
    a.sign = 1;
    return a;
}

// Add fixed-precision values together.
//
// This is surprisingly not straightforward.
//
// As usual, the first step is computing the lowest significant part
// and its carry-out. What's tricky here is that the carry might
// represent an ordinary carry, or it might be a borrow.
//
// In the case of a borrow, we need to subtract from the whole part
// instead of adding to it. That can be implemented by computing the
// sign of the decimal addition separately and multiplying it with the
// carry.
//
static FixedPrec fpAdd (FixedPrec a, FixedPrec b) {
    int decSign = a.decPart < b.decPart ? b.sign : a.sign;

    Int128 sum1 = adc64i
        ( a.sign * (int64_t) a.decPart
        , b.sign * (int64_t) b.decPart );

    Int128 sum2 = adc64i
        ( a.sign * (int64_t) a.wholePart
        , b.sign * (int64_t) b.wholePart +
            decSign * sum1.hi );

    int sign =
          a.wholePart < b.wholePart ? b.sign :
        ( a.decPart < b.decPart     ? b.sign : a.sign );

    return (FixedPrec) {
        .sign       = sign,
        .wholePart  = sign * sum2.lo,
        .decPart    = sign * sum1.lo
    };
}

// Subtract fixed-precision values.
//
// Just take advantage of
//
//  a - b == a + -b
//
static inline FixedPrec fpSub (FixedPrec a, FixedPrec b) {
    b.sign = -b.sign;
    return fpAdd (a, b);
}

// Multiply fixed-precision values together.
//
// Fixed-precision multiplication follows the exact same algebra as
// wide integer multiplication. See [WideInt.h:mul128] for details.
//
// Because of how our representation is structured, we know the scaling
// factor should always be 64, so we can just take the hi and lo bits
// of our 128-bit representation respectively.
//
static FixedPrec fpMul (FixedPrec a, FixedPrec b) {
    /*
    uint64_t    prod1 = a.wholePart * b.wholePart;
    UInt128     prod2 = mul64u (a.wholePart, b.decPart);
    UInt128     prod3 = mul64u (a.decPart, b.wholePart);
    UInt128     prod4 = mul64u (a.decPart, b.decPart);

    return (FixedPrec) {
        .sign       = a.sign * b.sign,
        .wholePart  = prod1 + prod2.hi + prod3.hi,
        .decPart    = prod2.lo + prod3.lo + prod4.hi
    };
    */

    UInt128 intProd = mul64u
        ( newUInt128 (a.wholePart, a.decPart)
        , newUInt128 (b.wholePart, b.decPart) );

    return (FixedPrec) {
        .sign       = a.sign * b.sign,
        .wholePart  = intProd.hi,
        .decPart    = intProd.lo
    };
}

// Divide two fixed-precision values.
//
static FixedPrec fpDiv (FixedPrec a, FixedPrec b) {

}

// Square a fixed-precision value.
//
static inline FixedPrec fpSqr (FixedPrec a) {
    return fpMul (a, a);
}

// Compute the square root of a fixed-precision value.
//
static FixedPrec fpSqrt (FixedPrec a) {

}


// 'Vec3' of 'FixedPrec's.
//
typedef struct Vec3FixedPrec Vec3FixedPrec;

struct Vec3FixedPrec {
    FixedPrec x;
    FixedPrec y;
    FixedPrec z;
};

// Compute the distance between two fixed-precision 'Vec3's.
//
static FixedPrec fp3Distance (Vec3FixedPrec a, Vec3FixedPrec b) {
    FixedPrec
        x2 = fpSqr (fpSub (a.x, b.x)),
        y2 = fpSqr (fpSub (a.y, b.y)),
        z2 = fpSqr (fpSub (a.z, b.z));

    return fpSqrt (fpAdd (fpAdd (x2, y2), z2));
}


// Convert a fixed-precision value to a float.
//
static float fpToFloat (FixedPrec a) {
    float decPart = (float) a.decPart / 0xFFFFFFFFFFFFFFFF;
    return a.sign * (a.wholePart + decPart);
}

#endif

