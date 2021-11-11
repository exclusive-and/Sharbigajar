
#ifndef SPACE_GAME_WIDE_INT_H
#define SPACE_GAME_WIDE_INT_H

#define MSB64(x)    (0x8000000000000000ULL & (x))

#define HI64(x)     (0x00000000FFFFFFFFULL & ((x) >> 32))
#define LO64(x)     (0x00000000FFFFFFFFULL & (x))

// 128-bit unsigned integer representation.
//
typedef struct UInt128 UInt128;

struct UInt128 {
    uint64_t hi;
    uint64_t lo;
};

// Create a new 128 bit unsigned integer.
//
static inline UInt128 newUInt128 (uint64_t hi, uint64_t lo) {
    return (UInt128) { hi, lo };
}

// Shift a 128-bit integer left by an arbitrary amount.
//
static inline UInt128 shl128u (UInt128 a, uint32_t n) {
    uint64_t carry =
        n < 64 ? a.lo >> (64 - n) : a.lo << (n - 64);

    return (UInt128) {
        .hi = (a.hi << n) | carry,
        .lo = a.lo << n
    };
}

// Shift a 128-bit integer right by an arbitrary amount.
//
static inline UInt128 shr128u (UInt128 a, uint32_t n) {
    uint64_t borrow =
        n < 64 ? a.hi << (64 - n) : a.hi >> (n - 64);

    return (UInt128) {
        .hi = a.hi >> n,
        .lo = (a.lo >> n) | borrow
    };
}

// Add two 128-bit unsigned integers together.
//
static inline UInt128 add128u (UInt128 a, UInt128 b) {
    uint64_t
        sum1 = LO64(a.lo) + LO64(b.lo),
        sum2 = HI64(a.lo) + HI64(b.lo),
        sum3 = LO64(a.hi) + LO64(b.hi),
        sum4 = HI64(a.hi) + HI64(b.hi);

    return (UInt128) {
        .lo = (sum2 << 32) + sum1,
        .hi = (sum4 << 32) + sum3 + HI64(sum2)
    };
}

// Subtract two 128-bit unsigned integers.
//
// Returns 0 if second operand is greater than the first.
//
static inline UInt128 sub128u (UInt128 a, UInt128 b) {
    const uint64_t extra = 0x200000000ULL;

    uint64_t
        sub1 = extra + LO64(a.lo) - LO64(b.lo),
        sub2 = extra + HI64(a.lo) - LO64(b.lo),
        sub3 = extra + LO64(a.hi) - LO64(b.hi),
        sub4 = extra + HI64(a.hi) - LO64(b.hi);

    sub2 -= HI64(sub1) ^ 2;
    sub3 -= HI64(sub2) ^ 2;
    sub4 -= HI64(sub3) ^ 2;

    uint64_t nuke = HI64(sub4) >> 1;

    return (UInt128) {
        .lo = ((LO64(sub2) << 32) + LO64(sub1)) * nuke,
        .hi = ((LO64(sub4) << 32) + LO64(sub3)) * nuke
    };
}

// Divide two 128-bit unsigned integers.
//
typedef struct Quotient128 Quotient128;

struct Quotient128 {
    UInt128 quotient;
    UInt128 remainder;
};

static inline Quotient128 div128u (UInt128 a, UInt128 b) {
    // Exceptional case
    if (b.hi == 0 && b.lo == 0) {
        printf ("error: div128u: divide by zero exception\n");
        return b;
    }

    UInt128
        quotient    = newUInt128 (0, 0),
        remainder   = newUInt128 (0, 0);

    for (uint32_t ix = 0; ix < 128; ix++) {
        quotient = shl128u (quotient, 1);
        remainder = shl128u (remainder, 1);

        remainder.lo |= shr128u (a, ix).lo & 1;

        if (lt128u (remainder, b)) {
            remainder = sub128u (remainder, b);
            quotient = add128u (quotient, 1);
        }
    }

    return (Quotient128) {
        .quotient   = quotient,
        .remainder  = remainder
    };
}


// Add together two 64-bit integers, and encode the carry overflow
// result in the highest 64 bits of 'UInt128'.
//
// Straightforwardly, a 64-bit addition is equivalent to two 32-bit
// additions with a carry signal between them.
//
// We separate the 64-bit inputs into 32-bit halves and add each
// half individually.
//
// Crucially, we use 64-bit addition for the halves, so that any overflow
// beyond the lower 32 bits ends up in the upper 32 bits. To compute the
// result of the carry between each half, we need only add the upper bits
// of the bottom half to the top half.
//
// If the top half overflows after all this, its own upper bits will
// provide us exactly the carry-out we're looking for.
//
static inline UInt128 adc64u (uint64_t a, uint64_t b) {
    uint64_t
        sum1 = LO64(a) + LO64(b),
        sum2 = HI64(a) + HI64(b);

    sum2 += HI64(sum1);

    return (UInt128) {
        .lo = (sum2 << 32) + sum1,
        .hi = HI64(sum2)
    }
}

// Multiply two 64-bit integers together to get a 128-bit integer.
//
// Let 'a = x + u' and 'b = y + v'.
// We can do a little bit of algebra to find that
//
//  a * b == (x + u) * (y + v) == x * y + x * v + u * y + u * v
//
// The same concept holds true for 64-bit integers.
// Let 'a = aHi << 32 + aLo' and 'b = bHi << 32 + bLo', then
//
//  a * b == aHi * bHi << 64 + aHi * bLo << 32 +
//           aLo * bHi << 32 + aLo * bLo
//
// Notice that each of the partial products is 64 bits, so there are
// 32 bits over overlap between 'aHi * bLo << 32 + aLo * bHi << 32' and
// both of 'aHi * bHi << 64' and 'aLo * bLo' respectively.
//
// The overlap is equivalent to adding the upper 32 bits of the middle
// product to the highest product, and then adding the remaining lower
// 32 bits to the lowest product.
//
// Our procedure for multiplying 64-bit integers is:
//  - disassemble each into 32-bit halves;
//  - compute the four partial products of these halves;
//  - get the overlaps to be added to each significant term;
//  - add everything up and return.
//
static inline UInt128 mul64u (uint64_t a, uint64_t b) {
    uint64_t
        prod1 = LO64(a) * LO64(b),
        prod2 = HI64(a) * LO64(b),
        prod3 = LO64(a) * HI64(b),
        prod4 = HI64(a) * HI64(b);

    uint64_t
        middle  = prod2 + prod3,
        over1   = LO64(middle) << 32,
        // ^ overlap highest bits of 'prod1' with lowest bits of
        // 'prod2 + prod3'.
        over4   = HI64(middle);
        // ^ overlap lowest bits of 'prod4' with highest bits of
        // 'prod2 + prod3'.

    return (UInt128) {
        .lo = prod1 + over1,
        .hi = prod4 + over4
    };
}


// 128-bit signed integer representation.
//
typedef struct Int128 Int128;

struct Int128 {
    int64_t hi;
    int64_t lo;
};

// Create an 'Int128' from two 64-bit integers.
//
// The final result will inherit the sign of the top half.
//
static inline newInt128 (int64_t top, uint64_t bottom) {
    int sign = top < 0 ? -1 : 1;

    return (Int128) {
        .lo = sign * bottom,
        .hi = top
    };
}

// Add two 64-bit signed integers, and encode carry overflow in the
// highest 64 bits of our 'Int128'.
//
// Getting down and dirty with hardware for a moment, we know that
// signed integers are encoded as unsigned integers with the 2's complement
// scheme. 2's complement is designed so that unsigned addition of encoded
// signed integers is the same as signed addition of unencoded integers.
//
// The upshot is that we can cast the inputs to 64 bit unsigned integers,
// perform our unsigned addition on them, and then cast them back to
// signed integers.
//
static inline Int64_t adc64i (int64_t a, int64_t b) {
    UInt128 ones = adc64u ((uint64_t) a, (uint64_t) b);

    return (Int64_t) {
        .lo = (int64_t) ones.lo,
        .hi = (int64_t) ones.hi
    };
}

#endif

