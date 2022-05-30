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
    kGalaxyColumns = 9,

    kCourseNotSet = -1
};

typedef enum Condition {
    kConditionGreen,
    kConditionYellow,
    kConditionRed
} Condition;

typedef enum SectorObject {
    kEnterprise = 'E',
    kKlingon = 'K',
    kStar = '*',
    kBase = 'B',
    kTorp = '#'
} SectorObject;

typedef struct Galaxy {
    Coordinate enterpriseSector;
    Coordinate enterpriseCoordinate;

    int timeLeft; // 300 == 3 years
    int stardate;  // e.g. 34240
    int shields; // 0..100 (%)
    Condition condition;
    int shieldPercentage; // 0..100 (%)
    int shieldEnergy;
    int availableEnergy;
    int torps;
    int klingons;
    int bases;
    Coordinate course; // kCourseNotSet / -1,-1 for unset course

    Sector sectors[kGalaxyRows][kGalaxyColumns];
    bool visible[kGalaxyRows][kGalaxyColumns];

    
} Galaxy;

void galaxyMakeVisibleAroundSector(Galaxy *galaxy, Coordinate sector);

// Hardcoded randomization.
void galaxyRandomize(Galaxy *galaxy, int baseCount, int klingonCount);
void galaxyPrint(Galaxy *galaxy);

#endif // TREK_INCLUDED
