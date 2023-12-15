#ifndef COLORS_H
#define COLORS_H

#include <stdio.h>
#include <stdlib.h>

void yuv2rgb(int width, int height, int maxGrayValue, FILE *inputFile, FILE *outputFile);

#endif // COLORS_H