#include "format.h"

int main() {
    const char *inputFilename = "/home/jeanf/Documents/S9/TVID/Projet/TVID/tools/mpeg2dec/255.pgm";
    const char *outputFilename = "output.ppm";

    pgmToPpm(inputFilename, outputFilename);

    printf("Conversion terminée.\n");

    return 0;
}
