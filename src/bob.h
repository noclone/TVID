#ifndef BOB_H
#define BOB_H

#include "header.h"

typedef struct {
    unsigned char R;
    unsigned char G;
    unsigned char B;
} Color;

void deinterlaceBob(const char *inputFilename, Header *header, int frame_number, const char *outputFilenameA, const char *outputFilenameB);
void deinterlaceAdaptive(const char *inputFilename, Header *header, int frame_number, const char *outputFilenameA, const char *outputFilenameB, int motionThreshold);

#endif // BOB_H