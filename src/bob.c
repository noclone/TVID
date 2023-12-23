#include "bob.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

void copyFile(const char *inputFilename, const char *outputFilename) {
    FILE *inputFile = fopen(inputFilename, "rb");
    FILE *outputFile = fopen(outputFilename, "wb");

    if (inputFile == NULL || outputFile == NULL) {
        perror("Erreur lors de l'ouverture des fichiers");
        exit(EXIT_FAILURE);
    }

    char c;
    while ((c = fgetc(inputFile)) != EOF) {
        fputc(c, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);
}

void deinterlaceBob(const char *inputFilename, Header *header, int frame_number, const char *outputFilenameA, const char *outputFilenameB) {

    FILE *inputFile = fopen(inputFilename, "rb");

    FILE *outputFileA = fopen(outputFilenameA, "wb");
    FILE *outputFileB = fopen(outputFilenameB, "wb");

    if (inputFile == NULL || outputFileA == NULL || outputFileB == NULL) {
        perror("Erreur lors de l'ouverture des fichiers");
        exit(EXIT_FAILURE);
    }

    char magicNumber[3];
    int width, height, maxColorValue;

    fscanf(inputFile, "%2c", magicNumber);
    fscanf(inputFile, "%d %d %d", &width, &height, &maxColorValue);

    if (magicNumber[0] != 'P' || magicNumber[1] != '6') {
        fprintf(stderr, "Format d'image non pris en charge. Seul PPM est pris en charge.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(outputFileA, "P6\n%d %d\n%d\n", width, height, maxColorValue);
    fprintf(outputFileB, "P6\n%d %d\n%d\n", width, height, maxColorValue);

    FILE *outputFile;
    char *line = malloc(sizeof(char) * width * 3);
    for (int i = 0; i < height; ++i) {
        if (i % 2 == !header->tffs[frame_number]) {
            outputFile = outputFileA;
        } else {
            outputFile = outputFileB;
        }
        for (int j = 0; j < width * 3; ++j) {
            fscanf(inputFile, "%c", &line[j]);
        }
        fwrite(line, sizeof(char), width * 3, outputFile);
        fwrite(line, sizeof(char), width * 3, outputFile);
    }

    fclose(inputFile);
    fclose(outputFileA);
    fclose(outputFileB);
    free(line);
}