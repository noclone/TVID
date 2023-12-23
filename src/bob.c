#include "bob.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

void deinterlaceBob(const char *inputFilename, Header *header, int frame_number, char* outputFolder, char* fileName) {

    char *subFolder = "bob";
    char *subFolderName = malloc(sizeof(char) * 1000);
    sprintf(subFolderName, "%s/%s", outputFolder, subFolder);
    if (access(subFolderName, F_OK) == -1) {
        mkdir(subFolderName, 0777);
    }

    FILE *inputFile = fopen(inputFilename, "rb");

    const char *dotPosition = strchr(fileName, '.');
    size_t length = dotPosition - fileName;
    char frameID[length + 1];
    strncpy(frameID, fileName, length);
    frameID[length] = '\0';

    char *outputFilenameA = malloc(sizeof(char) * 1000);
    sprintf(outputFilenameA, "%s/%s/%s_A.ppm", outputFolder, subFolder, frameID);
    char *outputFilenameB = malloc(sizeof(char) * 1000);
    sprintf(outputFilenameB, "%s/%s/%s_B.ppm", outputFolder, subFolder, frameID);

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
        if (i % 2 == 0) {
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