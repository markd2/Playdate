// Trek data structures

#ifndef TREK_INCLUDED
#define TREK_INCLUDED

#include <stdbool.h>

typedef struct Sector {
    short klingonCount;
    short baseCount;
    short starCount;
} Sector;

typedef struct Coordinate {
    short row;
    short column;
} Coordinate;
bool coordinateEqual(Coordinate thing1, Coordinate thing2);


enum {
    kGalaxyRows = 9,
    kGalaxyColumns = 9
};

typedef struct Galaxy {
    Coordinate enterpriseSector;
    Coordinate enterpriseCoordinate;

    Sector sectors[kGalaxyRows][kGalaxyColumns];
    bool visible[kGalaxyRows][kGalaxyColumns];
} Galaxy;

void galaxyMakeVisibleAroundSector(Galaxy *galaxy, Coordinate sector);

// Hardcoded randomization.
void galaxyRandomize(Galaxy *galaxy, int baseCount, int klingonCount);
void galaxyPrint(Galaxy *galaxy);

#endif // TREK_INCLUDED
