// Trek data structures

#ifndef TREK_INCLUDED
#define TREK_INCLUDED

typedef struct Sector {
    short klingonCount;
    short baseCount;
    short starCount;
} Sector;


typedef struct Galaxy {
    Sector sectors[8][10];  // 8 rows of 10 columns
} Galaxy;

#endif // TREK_INCLUDED
