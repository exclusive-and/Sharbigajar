
#ifndef SPACE_GAME_WIDE_INT_H
#define SPACE_GAME_WIDE_INT_H

#define HI64(x)     (0x00000000FFFFFFFF & (x >> 32))
#define LO64(x)     (0x00000000FFFFFFFF & x)

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

// Divide two 128-bit unsigned integers.
//
static inline UInt128 div128u (UInt128 a, UInt128 b) {
    uint64_t
        work1       = a.hi,
        work2       = a.lo,
        remainder   = 0;

    for (int ix = 64; ix > 0; ix--) {
        remainder <<= 1;

        if (work2 & 1)
            remainder++;
        work2 >>= 1;
    }

    uint64_t quot1 = remainder / b.hi;
    remainder -= quot1 * b.hi;

    for (int ix = 64; ix > 0; ix--) {
        quot1 <<= 1;
        remainder <<= 1;

        if (work1 & 1)
            remainder++;
        work1 >>= 1;
    }

    uint64_t quot2 = remainder / b.lo;
    remainder -= quot2 * b.lo;

    return newUInt128 (quot1 + quot2, remainder);
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

