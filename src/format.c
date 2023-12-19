#include "format.h"
#include "colors.h"

void pgmToPpm(const char *inputFilename, const char *outputFilename) {

    FILE *inputFile = fopen(inputFilename, "rb");
    FILE *outputFile = fopen(outputFilename, "wb");

    if (inputFile == NULL || outputFile == NULL) {
        perror("Erreur lors de l'ouverture des fichiers");
        exit(EXIT_FAILURE);
    }

    char magicNumber[3];
    int width, height, maxColorValue;

    fscanf(inputFile, "%2s", magicNumber);
    fscanf(inputFile, "%d %d %d", &width, &height, &maxColorValue);

    if (magicNumber[0] != 'P' || magicNumber[1] != '5') {
        fprintf(stderr, "Format d'image non pris en charge. Seul PGM est pris en charge.\n");
        exit(EXIT_FAILURE);
    }

    yuv2rgb(width, height, maxColorValue, inputFile, outputFile);

    fclose(inputFile);
    fclose(outputFile);
}
