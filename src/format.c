#include "format.h"

void pgmToPpm(const char *inputFilename, const char *outputFilename) {

    FILE *inputFile = fopen(inputFilename, "rb");
    FILE *outputFile = fopen(outputFilename, "wb");

    if (inputFile == NULL || outputFile == NULL) {
        perror("Erreur lors de l'ouverture des fichiers");
        exit(EXIT_FAILURE);
    }

    char magicNumber[3];
    int width, height, maxGrayValue;

    fscanf(inputFile, "%2s", magicNumber);
    fscanf(inputFile, "%d %d %d", &width, &height, &maxGrayValue);

    if (magicNumber[0] != 'P' || magicNumber[1] != '5') {
        fprintf(stderr, "Format d'image non pris en charge. Seul PGM est pris en charge.\n");
        exit(EXIT_FAILURE);
    }

    unsigned char *yuvPixels = (unsigned char *)malloc((width * height) * sizeof(unsigned char));
    fread(yuvPixels, sizeof(unsigned char), (width * height), inputFile);

    height = (int)((float)height * (2.0/3.0));
    fprintf(outputFile, "P6\n%d %d\n%d\n", width, height, maxGrayValue);

    int uvOffset = width * height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int Y = yuvPixels[y * width + x];
            int U = yuvPixels[uvOffset + (y / 2) * (width / 2) + (x / 2)];
            int V = yuvPixels[uvOffset + (width * height / 4) + (y / 2) * (width / 2) + (x / 2)];

            int C = Y - 16;
            int D = U - 128;
            int E = V - 128;

            int R = (int)(1.164 * C + 1.596 * E);
            int G = (int)(1.164 * C - 0.392 * D - 0.813 * E);
            int B = (int)(1.164 * C + 2.017 * D);

            R = (R < 0) ? 0 : ((R > 255) ? 255 : R);
            G = (G < 0) ? 0 : ((G > 255) ? 255 : G);
            B = (B < 0) ? 0 : ((B > 255) ? 255 : B);

            fputc(R, outputFile);  // Red
            fputc(G, outputFile);  // Green
            fputc(B, outputFile);  // Blue
        }
    }
    free(yuvPixels);


    fclose(inputFile);
    fclose(outputFile);
}
