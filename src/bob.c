#include "bob.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

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

    if (magicNumber[0] != 'P' || magicNumber[1] != '5') {
        fprintf(stderr, "Format d'image non pris en charge. Seul PGM est pris en charge.\n");
        exit(EXIT_FAILURE);
    }

    unsigned char *yuvPixels = (unsigned char *)malloc((width * height) * sizeof(unsigned char));
    fread(yuvPixels, sizeof(unsigned char), (width * height), inputFile);

    int newHeight = height * 2 / 3;

    fprintf(outputFileA, "P6\n%d %d\n%d\n", width, newHeight, maxColorValue);
    fprintf(outputFileB, "P6\n%d %d\n%d\n", width, newHeight, maxColorValue);

    FILE *outputFile;
    unsigned char *line = malloc(sizeof(unsigned char) * width * 3);
    int idx;
    for (int y = 0; y < newHeight; y++) {
        idx = 0;
        if (y % 2 == !header->tffs[frame_number]) {
            outputFile = outputFileA;
        } else {
            outputFile = outputFileB;
        }
        for (int x = 0; x < width; x++) {
            int Y = yuvPixels[y * width + x];
            int U = yuvPixels[(newHeight + (y / 2)) * width + (x / 2)];
            int V = yuvPixels[(newHeight + (y / 2)) * width + (width / 2) + (x / 2)];

            int C = Y - 16;
            int D = U - 128;
            int E = V - 128;

            int R = (int)(C + 1.402 * E);
            int G = (int)(C - 0.344 * D - 0.714 * E);
            int B = (int)(C + 1.772 * D);

            R = (R < 0) ? 0 : ((R > 255) ? 255 : R);
            G = (G < 0) ? 0 : ((G > 255) ? 255 : G);
            B = (B < 0) ? 0 : ((B > 255) ? 255 : B);

            line[idx++] = (unsigned char)R;
            line[idx++] = (unsigned char)G;
            line[idx++] = (unsigned char)B;
        }
        fwrite(line, sizeof(unsigned char), width * 3, outputFile);
        fwrite(line, sizeof(unsigned char), width * 3, outputFile);
    }

    fclose(inputFile);
    fclose(outputFileA);
    fclose(outputFileB);
    free(line);
}