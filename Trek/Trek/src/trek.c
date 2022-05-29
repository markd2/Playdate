#include "trek.h"
#include <string.h> // for memset
#include <stdlib.h> // for arc4random_uniform
#include "globals.h" // for print

bool coordinateEqual(Coordinate thing1, Coordinate thing2) {
    return thing1.row == thing2.row && thing1.column == thing2.column;
} // coordinateEqual


void galaxyRandomize(Galaxy *galaxy, int baseCount, int klingonCount) {
    memset(galaxy, 0, sizeof(*galaxy));

    for (short row = 0; row < kGalaxyRows; row++) {
        for (short column = 0; column < kGalaxyColumns; column++) {
            galaxy->sectors[row][column].starCount = random() % 10;
        }
    }

    for (int base = 0; base < baseCount; base++) {
        short row = random() % kGalaxyRows;
        short column = random() % kGalaxyColumns;
        galaxy->sectors[row][column].baseCount++;
    }

    // TODO: This is too dispersed. have bigger concentrations.
    for (int klingon = 0; klingon < klingonCount; klingon++) {
        short row = random() % kGalaxyRows;
        short column = random() % kGalaxyColumns;
        galaxy->sectors[row][column].klingonCount++;
    }

} // galaxyRandomize


void galaxyPrint(Galaxy *galaxy) {
    char line[1024];
    char *lineScan;

    for (short row = 0; row < kGalaxyRows; row++) {
        lineScan = line;
        for (short column = 0; column < kGalaxyColumns; column++) {
            Sector sector = galaxy->sectors[row][column];
            *lineScan++ = sector.klingonCount + '0';
            *lineScan++ = sector.baseCount + '0';
            *lineScan++ = sector.starCount + '0';

            *lineScan++ = ' ';
            *lineScan++ = ':';
            *lineScan++ = ' ';
        }
        *(lineScan - 2) = '\000';
        print(line);
    }

} // galaxyPrint
