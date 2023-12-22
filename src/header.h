#ifndef HEADER_H
#define HEADER_H

typedef struct header {
    int size;
    int *progs;
    int *tffs;
    int *rffs;
    int *period_changes_indices;
    int *period_changes_values;
} header;

void parseHeaderFile(char* headerFileName, header* header);

#endif // HEADER_H