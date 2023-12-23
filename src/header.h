#ifndef HEADER_H
#define HEADER_H

typedef struct {
    int size;
    int *progs;
    int *tffs;
    int *rffs;
    int *period_changes_indices;
    int *period_changes_values;
} Header;

void parseHeaderFile(char* headerFileName, Header* header);

#endif // HEADER_H