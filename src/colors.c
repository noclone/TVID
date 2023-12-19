#include "colors.h"

void yuv2rgb(int width, int height, int maxColorValue, FILE *inputFile, FILE *outputFile){

    unsigned char *yuvPixels = (unsigned char *)malloc((width * height) * sizeof(unsigned char));
    fread(yuvPixels, sizeof(unsigned char), (width * height), inputFile);

    int newHeight = height * 2 / 3;
    fprintf(outputFile, "P6\n%d %d\n%d\n", width, newHeight, maxColorValue);

    for (int y = 0; y < newHeight; y++) {
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

            fputc((unsigned char)R, outputFile);
            fputc((unsigned char)G, outputFile);
            fputc((unsigned char)B, outputFile);

            if (x == 0)
                printf("rgb: %d %d %d\n", R, G, B);
        }
    }
    
    free(yuvPixels);
}