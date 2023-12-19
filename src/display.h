#ifndef DISPLAY_H
#define DISPLAY_H

#include <dirent.h>

void display(const char* inputFolder, int num_entries, struct dirent **namelist, int frame_rate);

#endif // DISPLAY_H