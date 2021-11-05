
#ifndef SPACE_GAME_NEWTONIAN_H
#define SPACE_GAME_NEWTONIAN_H

#include "FixedPrecision.h"

// Store physics properties of a Newtonian body.
//
typedef struct Newtonian Newtonian;

struct Newtonian {
    // Intrinsic characteristics
    FixedPrec mass;

    // Newtonian data
    Vec3FixedPrec position;
    Vec3FixedPrec velocity;
};

Vec3FixedPrec gravity (Newtonian, Newtonian);

Newtonian applyForce (Vec3FixedPrec, Newtonian);
