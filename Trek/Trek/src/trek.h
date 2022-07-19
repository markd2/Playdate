// Trek data structures

#ifndef TREK_INCLUDED
#define TREK_INCLUDED

#include <stdbool.h>

enum {
    kGalaxyRows = 9,
    kGalaxyColumns = 9,

    kSectorRows = 8,
    kSectorColumns = 8,

    kCourseNotSet = -1
};

typedef struct Coordinate {
    short row;
    short column;
} Coordinate;
bool coordinateEqual(Coordinate thing1, Coordinate thing2);


typedef struct SectorInfo {
    short klingonCount;
    short baseCount;
    short starCount;
} SectorInfo;

typedef enum Condition {
    kConditionGreen,
    kConditionYellow,
    kConditionRed
} Condition;

typedef enum SectorObject {
    kEmpty = 0,
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

    SectorInfo sectors[kGalaxyRows][kGalaxyColumns];
    bool visible[kGalaxyRows][kGalaxyColumns];
   
} Galaxy;

void galaxyMakeVisibleAroundSector(Galaxy *galaxy, Coordinate sector);

// Hardcoded randomization.
void galaxyRandomize(Galaxy *galaxy, int baseCount, int klingonCount);
void galaxyPrint(Galaxy *galaxy);


typedef struct SectorDetail {
    SectorObject objects[kSectorRows][kSectorColumns];
} SectorDetail;



#endif // TREK_INCLUDED
