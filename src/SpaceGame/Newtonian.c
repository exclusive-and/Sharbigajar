
// SpaceGame.Newtonian

#include <stdio.h>
#include <stdlib.h>

#include "FixedPrecision.h"
#include "Newtonian.h"



// Compute the gravitational force between two Newtonian bodies.
//
Vec3FixedPrec gravity (Newtonian newt1, Newtonian newt2) {
    FixedPrec r = fp3Distance (newt1.position, newt2.position);
    return G * newt1.mass * newt2.mass / (r * r);
}
