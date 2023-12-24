#ifndef BOB_H
#define BOB_H

#include "header.h"

void deinterlaceBob(const char *inputFilename, Header *header, int frame_number, const char *outputFilenameA, const char *outputFilenameB);

#endif // BOB_H