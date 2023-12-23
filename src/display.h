#ifndef DISPLAY_H
#define DISPLAY_H

#include <dirent.h>
#include "header.h"

void display(const char* inputFolder, int num_entries, struct dirent **namelist, int frame_rate, Header *header);

#endif // DISPLAY_H