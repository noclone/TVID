#ifndef DISPLAY_H
#define DISPLAY_H

#include <dirent.h>

void display(const char* inputFolder, const char*outputFolder, int num_entries, struct dirent **namelist);

#endif // DISPLAY_H