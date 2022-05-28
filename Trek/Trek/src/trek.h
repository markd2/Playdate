// Trek data structures

#ifndef TREK_INCLUDED
#define TREK_INCLUDED

typedef struct Sector {
    short klingonCount;
    short baseCount;
    short starCount;
} Sector;

typedef struct Coordinate {
    short row;
    short column;
} Coordinate;

enum {
    kGalaxyRows = 9,
    kGalaxyColumns = 9
};

typedef struct Galaxy {
    Coordinate enterpriseSector;
    Coordinate enterpriseCoordinate;

    Sector sectors[kGalaxyRows][kGalaxyColumns];
} Galaxy;

// Hardcoded randomization.
void galaxyRandomize(Galaxy *galaxy, int baseCount, int klingonCount);
void galaxyPrint(Galaxy *galaxy);

#endif // TREK_INCLUDED
