#include "bob.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

Color getRGBfromYUV(unsigned char *pixels, int x, int y, int width, int height){
    int Y = pixels[y * width + x];
    int U = pixels[(height + (y / 2)) * width + (x / 2)];
    int V = pixels[(height + (y / 2)) * width + (width / 2) + (x / 2)];

    int C = Y - 16;
    int D = U - 128;
    int E = V - 128;

    int R = (int)(C + 1.402 * E);
    int G = (int)(C - 0.344 * D - 0.714 * E);
    int B = (int)(C + 1.772 * D);

    R = (R < 0) ? 0 : ((R > 255) ? 255 : R);
    G = (G < 0) ? 0 : ((G > 255) ? 255 : G);
    B = (B < 0) ? 0 : ((B > 255) ? 255 : B);

    Color color = {(unsigned char)R, (unsigned char)G, (unsigned char)B};
    return color;
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

            Color color = getRGBfromYUV(yuvPixels, x, y, width, newHeight);

            line[idx++] = color.R;
            line[idx++] = color.G;
            line[idx++] = color.B;
        }
        fwrite(line, sizeof(unsigned char), width * 3, outputFile);
        fwrite(line, sizeof(unsigned char), width * 3, outputFile);
    }

    fclose(inputFile);
    fclose(outputFileA);
    fclose(outputFileB);
    free(line);
    free(yuvPixels);
}

int motionDetection(unsigned char *currentLine, unsigned char *previousLine, int width, int motionThreshold) {
    int sumSquaredDifference = 0;

    for (int i = 0; i < width; i++) {
        int difference = currentLine[i] - previousLine[i];
        sumSquaredDifference += difference * difference;
    }

    int averageSquaredDifference = sumSquaredDifference / width;
    return averageSquaredDifference > (motionThreshold * motionThreshold);
}

void deinterlaceAdaptive(const char *inputFilename, Header *header, int frame_number, const char *outputFilenameA, const char *outputFilenameB, int motionThreshold) {

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

    FILE *previousFrame;
    unsigned char *yuvPixelsPrevious = (unsigned char *)malloc((width * height) * sizeof(unsigned char));
    if (frame_number != 0){
        char *previousFrameName = malloc(sizeof(char) * 1000);
        char *lastSlash = strrchr(inputFilename, '/');
        strncpy(previousFrameName, inputFilename, lastSlash - inputFilename);
        sprintf(previousFrameName + (lastSlash - inputFilename), "/%d.pgm", frame_number - 1);
        previousFrame = fopen(previousFrameName, "rb");
        char magicNumber2[3];
        int width2, height2, maxColorValue2;
        fscanf(previousFrame, "%2c", magicNumber2);
        fscanf(previousFrame, "%d %d %d", &width2, &height2, &maxColorValue2);
        if (magicNumber2[0] != 'P' || magicNumber2[1] != '5') {
            fprintf(stderr, "Format d'image non pris en charge. Seul PGM est pris en charge.\n");
            exit(EXIT_FAILURE);
        }
        fread(yuvPixelsPrevious, sizeof(unsigned char), (width * height), previousFrame);
        fclose(previousFrame);
    }

    int newHeight = height * 2 / 3;

    fprintf(outputFileA, "P6\n%d %d\n%d\n", width, newHeight, maxColorValue);
    fprintf(outputFileB, "P6\n%d %d\n%d\n", width, newHeight, maxColorValue);

    FILE *outputFile;
    unsigned char *line = malloc(sizeof(unsigned char) * width * 3);
    unsigned char *previousLine = malloc(sizeof(unsigned char) * width * 3);
    unsigned char *tmpLine = malloc(sizeof(unsigned char) * width * 3);
    unsigned char *oppositeFieldLine = malloc(sizeof(unsigned char) * width * 3);

    int segmentSize = 10;

    int idx;
    int previousIdx;
    int tmpLineIdx;
    for (int y = 0; y < newHeight; y++) {
        idx = 0;
        previousIdx = 0;
        tmpLineIdx = 0;
        if (y % 2 == !header->tffs[frame_number]) {
            outputFile = outputFileA;
        } else {
            outputFile = outputFileB;
        }
        for (int x = 0; x < width; x++) {

            Color color = getRGBfromYUV(yuvPixels, x, y, width, newHeight);

            line[idx++] = color.R;
            line[idx++] = color.G;
            line[idx++] = color.B;

            tmpLine[tmpLineIdx++] = color.R;
            tmpLine[tmpLineIdx++] = color.G;
            tmpLine[tmpLineIdx++] = color.B;

            if (frame_number != 0){
                Color previousColor = getRGBfromYUV(yuvPixelsPrevious, x, y, width, newHeight);
                previousLine[previousIdx++] = previousColor.R;
                previousLine[previousIdx++] = previousColor.G;
                previousLine[previousIdx++] = previousColor.B;
            }

            if (frame_number != 0 && idx == segmentSize * 3 - 1){
                if (motionDetection(line, previousLine, idx, motionThreshold)) {
                    fwrite(line, sizeof(unsigned char), idx, outputFile);
                    fwrite(line, sizeof(unsigned char), idx, outputFile);
                } else {
                    fwrite(line, sizeof(unsigned char), idx, outputFile);
                    fwrite(oppositeFieldLine + tmpLineIdx - idx, sizeof(unsigned char), idx, outputFile);
                }
                idx = 0;
                previousIdx = 0;
            }

        }

        if (frame_number == 0){
            fwrite(line, sizeof(unsigned char), width * 3, outputFile);
            fwrite(line, sizeof(unsigned char), width * 3, outputFile);
        }
        else if (idx != 0){
            if (motionDetection(line, previousLine, idx, motionThreshold)) {
                fwrite(line, sizeof(unsigned char), idx, outputFile);
                fwrite(line, sizeof(unsigned char), idx, outputFile);
            } else {
                fwrite(line, sizeof(unsigned char), idx, outputFile);
                fwrite(oppositeFieldLine + tmpLineIdx - idx, sizeof(unsigned char), idx, outputFile);
            }
        }

        memcpy(oppositeFieldLine, tmpLine, width * 3);

    }

    fclose(inputFile);
    fclose(outputFileA);
    fclose(outputFileB);
    free(line);
    free(previousLine);
    free(yuvPixels);
    free(yuvPixelsPrevious);
    free(tmpLine);
    free(oppositeFieldLine);
}
